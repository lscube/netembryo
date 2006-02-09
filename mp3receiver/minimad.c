/* * 
 *  $Id$
 *  
 *  This file is part of NetEmbryo 
 *
 * NetEmbryo -- default network wrapper 
 *
 *  Copyright (C) 2005 by
 *  	
 *	- Federico Ridolfo	<federico.ridolfo@polito.it>
 * 
 *  NetEmbryo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NetEmbryo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NetEmbryo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 * */

# include <stdio.h>
# include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


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
	int bytes_to_preserve = stream->bufend - stream->next_frame;
	unsigned char buf[ MAX_BUFFER + MAD_BUFFER_GUARD];

	if (po->potail  < 0) {
		//fprintf(stderr,"po->potail= -1\n");
		return MAD_FLOW_STOP;
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
static unsigned short cazcount;

enum mad_flow madoutput(void *data, struct mad_header const *header, struct mad_pcm *pcm)
{
	struct buffer *buffer = (struct buffer *) data;
	int vol = 100;
	int volume = (vol/100.0) * MAD_F_ONE;

	register int nsamples = pcm->length;
	mad_fixed_t const *left_ch = pcm->samples[0], *right_ch = pcm->samples[1];

	static unsigned char stream[1152*4]; /* 1152 because that's what mad has as a max; *4 because
                                there are 4 distinct bytes per sample (in 2 channel case) */
	static unsigned int rate = 0;
	static int channels = 0;
	static struct audio_dither dither;

	register char * ptr = stream;
	register signed int sample;
	register mad_fixed_t tempsample;

	
#if ENABLE_DEBUG
	char cazzatine[4] = { '\\' , '|' , '/' , '-'};
#endif //ENABLE_DEBUG

#if ENABLE_LIBAO
	int ao_id=0;
	ao_sample_format format; //ao
	ao_option *options=NULL; //ao
	ao_device *ao_dev; //ao
#else
	Sound_Handle hand=NULL;
	int direction = O_WRONLY;
#endif	// ENABLE_LIBAO	


	/* We need to know information about the file before we can open the playdevice
	   in some cases. So, we do it here. */
	if (!buffer->playdevice) {
		channels = MAD_NCHANNELS(header);
		rate = header->samplerate;
#if ENABLE_LIBAO
		ao_initialize(); //ao
		if((ao_id=ao_default_driver_id())<0) {//ao
			fprintf(stderr,"ao_default_driver_id error using default oss\n");
			ao_id = ao_driver_id("oss");
		}

		format.channels=channels;
		format.bits=16;
		format.rate=rate;
		ao_dev=ao_open_live(ao_id, &format, options);
		buffer->ao_dev = ao_dev;
		fprintf(stderr," - libao enabled\n\n");
#else
		sound_init();
		set_sound_duplex(FULL_DUPLEX);
		set_sound_eight_bit(0);
	
		if(channels==2)
			set_stereo_mode();
		set_speed(rate);
		hand = sound_open(direction);
		buffer->hand = hand;
#endif	
		buffer->playdevice = 1;
	}


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

#if ENABLE_DEBUG
		 fprintf(stderr, "[MPA] bitrate: %ld - sample rate: %d - channels: %d - pcm->length: %4d - buffer: %3d%%  [%c]\r", \
				header->bitrate, \
				rate, channels,\
				pcm->length,\
				buffer->bp->flcount*100/BP_SLOT_NUM, \
				cazzatine[(cazcount++)%4]);
#endif //ENABLE_DEBUG
	/*if (buffer->bp->flcount > BP_SLOT_NUM - 2) { 
		mad_timer_set(&(buffer->timer),0,0,0);
		mad_timer_add(&(buffer->timer), header->duration);
	}*/
	
	//fprintf(stderr,"%f\n",header->duration);

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
