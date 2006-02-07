/*
 * libmad - MPEG audio decoder library
 * Copyright (C) 2000-2003 Underbit Technologies, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: minimad.c,v 1.2 2003/06/05 02:27:07 rob Exp $
 */

# include <stdio.h>
# include <unistd.h>

# include <mad.h>
# include "minimad.h"
#include <programs/mp3receiver.h>

#define RTPHEADERSIZE 12
#define RTPEXTENSIONSIZE 4
#define HEADERSIZE ( RTPHEADERSIZE + RTPEXTENSIONSIZE )


/*
 * This is the input callback. The purpose of this callback is to (re)fill
 * the stream buffer which is to be decoded. In this example, an entire file
 * has been mapped into memory, so we just call mad_stream_buffer() with the
 * address and length of the mapping. When this callback is called a second
 * time, we are finished decoding.
 */

enum mad_flow madinput(void *data, struct mad_stream *stream)
{
	struct buffer *buffer = (struct buffer *) data;
	playout_buff *po = buffer->po;
	buffer_pool *bp = buffer->bp;
	struct timespec ts;
	int bytes_to_preserve = stream->bufend - stream->next_frame;
	unsigned char buf[ MAX_BUFFER + MAD_BUFFER_GUARD];

	if (po->potail == -1) {
		//fprintf(stderr,"po->potail= -1\n");
		return MAD_FLOW_CONTINUE;
	}

	if (bp->flcount <= 2) {
		//prefill    
		while (bp->flcount < DEFAULT_MIN_QUEUE) {
			//fprintf(stderr,"buffer = %d\n",bp->flcount);
			ts.tv_sec = 0;
			ts.tv_nsec = 26122 * DEFAULT_MIN_QUEUE * 1000;	//only to rescale the process
			nanosleep(&ts, NULL);
		}
	}

	if (bytes_to_preserve)
		memmove(buf, stream->next_frame, bytes_to_preserve);

	memcpy(buf + bytes_to_preserve, ((unsigned char *) (&(*po->bufferpool)[po->potail])) + HEADERSIZE,
	       (po->pobuff[po->potail]).pktlen - HEADERSIZE);

	mad_stream_buffer(stream, buf, (po->pobuff[po->potail]).pktlen - HEADERSIZE + bytes_to_preserve);
	//fprintf(stderr, "po->potail= %d\n", po->potail);

	bprmv(bp, po, po->potail);

	return MAD_FLOW_CONTINUE;
}

struct audio_dither {
	mad_fixed_t error[3];
	mad_fixed_t random;
};

/*
* NAME:        prng()
* DESCRIPTION: 32-bit pseudo-random number generator
*/
static inline unsigned long prng(unsigned long state)
{
	return (state * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;
}

/*
* NAME:        audio_linear_dither()
* DESCRIPTION: generic linear sample quantize and dither routine
*/
static inline signed long audio_linear_dither(unsigned int bits, mad_fixed_t sample, struct audio_dither *dither)
{
	unsigned int scalebits;
	mad_fixed_t output, mask, random;

	enum {
		MIN = -MAD_F_ONE,
		MAX = MAD_F_ONE - 1
	};

	/* noise shape */
	sample += dither->error[0] - dither->error[1] + dither->error[2];

	dither->error[2] = dither->error[1];
	dither->error[1] = dither->error[0] / 2;

	/* bias */
	output = sample + (1L << (MAD_F_FRACBITS + 1 - bits - 1));

	scalebits = MAD_F_FRACBITS + 1 - bits;
	mask = (1L << scalebits) - 1;

	/* dither */
	random = prng(dither->random);
	output += (random & mask) - (dither->random & mask);

	dither->random = random;

	/* clip */
	if (output > MAX) {
		output = MAX;

		if (sample > MAX)
			sample = MAX;
	} else if (output < MIN) {
		output = MIN;

		if (sample < MIN)
			sample = MIN;
	}

	/* quantize */
	output &= ~mask;

	/* error feedback */
	dither->error[0] = sample - output;

	/* scale */
	return output >> scalebits;
}

/*
 * This is the output callback function. It is called after each frame of
 * MPEG audio data has been completely decoded. The purpose of this callback
 * is to output (or play) the decoded PCM audio.
 */
enum mad_flow madoutput(void *data, struct mad_header const *header, struct mad_pcm *pcm)
{
	unsigned int nchannels, nsamples;
	mad_fixed_t const *left_ch, *right_ch;
	struct buffer *buffer = (struct buffer *) data;
	int volume = 100;

	static unsigned char stream[1152 * 4];	/* 1152 because that's what mad has as a max; *4 because
						   there are 4 distinct bytes per sample (in 2 channel case) */
	static unsigned int rate = 0;
	static int channels = 0;
	static struct audio_dither dither;

	register char *ptr = stream;
	register signed int sample;
	register mad_fixed_t tempsample;


	/* pcm->samplerate contains the sampling frequency */
	nchannels = pcm->channels;
	nsamples = pcm->length;
	left_ch = pcm->samples[0];
	right_ch = pcm->samples[1];



	//TODO
	/* We need to know information about the file before we can open the playdevice
	   in some cases. So, we do it here. */
	/*if (!playdevice)
	   {
	   channels = MAD_NCHANNELS(header);
	   rate = header->samplerate;
	   open_ao_playdevice(header);        
	   }

	   else if ((channels != MAD_NCHANNELS(header) || rate != header->samplerate) && playdevice_is_live())
	   {
	   ao_close(playdevice);
	   channels = MAD_NCHANNELS(header);
	   rate = header->samplerate;
	   open_ao_playdevice(header);        
	   } */
	if (pcm->channels == 2) {
		while (nsamples--) {
			tempsample = (mad_fixed_t) ((*left_ch++ * (double) volume) / MAD_F_ONE);
			sample = (signed int) audio_linear_dither(16, tempsample, &dither);

#ifndef WORDS_BIGENDIAN
			*ptr++ = (unsigned char) (sample >> 0);
			*ptr++ = (unsigned char) (sample >> 8);
#else
			*ptr++ = (unsigned char) (sample >> 8);
			*ptr++ = (unsigned char) (sample >> 0);
#endif

			tempsample = (mad_fixed_t) ((*right_ch++ * (double) volume) / MAD_F_ONE);
			sample = (signed int) audio_linear_dither(16, tempsample, &dither);
#ifndef WORDS_BIGENDIAN
			*ptr++ = (unsigned char) (sample >> 0);
			*ptr++ = (unsigned char) (sample >> 8);
#else
			*ptr++ = (unsigned char) (sample >> 8);
			*ptr++ = (unsigned char) (sample >> 0);
#endif
		}
#if ENABLE_LIBAO
		ao_play(buffer->ao_dev, stream, pcm->length * 4);
#else
		write_samples(buffer->hand, stream, pcm->length * 4);
#endif				//ENABLE_LIBAO
	}

	else {			/* Just straight mono output */

		while (nsamples--) {
			tempsample = (mad_fixed_t) ((*left_ch++ * (double) volume) / MAD_F_ONE);
			sample = (signed int) audio_linear_dither(16, tempsample, &dither);

#ifndef WORDS_BIGENDIAN
			*ptr++ = (unsigned char) (sample >> 0);
			*ptr++ = (unsigned char) (sample >> 8);
#else
			*ptr++ = (unsigned char) (sample >> 8);
			*ptr++ = (unsigned char) (sample >> 0);
#endif
		}
#if ENABLE_LIBAO
		ao_play(buffer->ao_dev, stream, pcm->length * 2);
#else
		write_samples(buffer->hand, stream, pcm->length * 2);
#endif				//ENABLE_LIBAO
	}

//#if ENABLE_DEBUG
		 fprintf(stderr, "[MPA] bitrate: %d - sample rate: %ld - buffer: %d%% - pcm->length: %6d\r", \
				header->bitrate, \
				header->samplerate, buffer->bp->flcount*100/BP_SLOT_NUM, pcm->length);
//#endif //ENABLE_DEBUG
	mad_timer_add(&(buffer->timer), header->duration);

	return MAD_FLOW_CONTINUE;
}

enum mad_flow madheader(void *data, struct mad_header const *header)
{

	return MAD_FLOW_CONTINUE;
}

/*
 * This is the error callback function. It is called whenever a decoding
 * error occurs. The error is indicated by stream->error; the list of
 * possible MAD_ERROR_* errors can be found in the mad.h (or
 * libmad/stream.h) header file.
 */

//static
enum mad_flow maderror(void *data, struct mad_stream *stream, struct mad_frame *frame)
{
	struct buffer *buffer = (struct buffer *) data;

	fprintf(stderr, "decoding error 0x%04x (%s) \n", stream->error, mad_stream_errorstr(stream));

	/* return MAD_FLOW_BREAK here to stop decoding (and propagate an error) */
	return MAD_FLOW_CONTINUE;
}
