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
# include "mpgao.h"

#define RTPHEADERSIZE 12
#define RTPEXTENSIONSIZE 4
#define HEADERSIZE ( RTPHEADERSIZE + RTPEXTENSIONSIZE )


/*
 * This is a private message structure. A generic pointer to this structure
 * is passed to each of the callback functions. Put here any data you need
 * to access from within the callbacks.
 */

struct buffer {
	playout_buff *po;
	buffer_pool *bp;
	Sound_Handle hand;
};

/*
 * This is the input callback. The purpose of this callback is to (re)fill
 * the stream buffer which is to be decoded. In this example, an entire file
 * has been mapped into memory, so we just call mad_stream_buffer() with the
 * address and length of the mapping. When this callback is called a second
 * time, we are finished decoding.
 */

static
enum mad_flow madinput(void *data, struct mad_stream *stream)
{
	struct buffer *buffer = data;
	playout_buff *po = buffer->po;
	buffer_pool *bp = buffer->bp;

	if (po->potail == -1)
		return MAD_FLOW_CONTINUE;

	mad_stream_buffer(stream, (void *) (&(*po->bufferpool)[po->potail]) + HEADERSIZE,
			  (po->pobuff[po->potail]).pktlen - HEADERSIZE);


	return MAD_FLOW_CONTINUE;
}

/*
 * The following utility routine performs simple rounding, clipping, and
 * scaling of MAD's high-resolution samples down to 16 bits. It does not
 * perform any dithering or noise shaping, which would be recommended to
 * obtain any exceptional audio quality. It is therefore not recommended to
 * use this routine if high-quality output is desired.
 */

static inline signed int scale(mad_fixed_t sample)
{
	/* round */
	sample += (1L << (MAD_F_FRACBITS - 16));

	/* clip */
	if (sample >= MAD_F_ONE)
		sample = MAD_F_ONE - 1;
	else if (sample < -MAD_F_ONE)
		sample = -MAD_F_ONE;

	/* quantize */
	return sample >> (MAD_F_FRACBITS + 1 - 16);
}

/*
 * This is the output callback function. It is called after each frame of
 * MPEG audio data has been completely decoded. The purpose of this callback
 * is to output (or play) the decoded PCM audio.
 */

static
enum mad_flow madoutput(void *data, struct mad_header const *header, struct mad_pcm *pcm)
{
	unsigned int nchannels, nsamples;
	mad_fixed_t const *left_ch, *right_ch;
	char cazzatine[4] = { '\\', '|', '/', '-' };
	unsigned short cazcount = 0;

	struct buffer *buffer = data;
	playout_buff *po = buffer->po;
	buffer_pool *bp = buffer->bp;
	Sound_Handle hand = buffer->hand;


	/* pcm->samplerate contains the sampling frequency */

	nchannels = pcm->channels;
	nsamples = pcm->length;
	left_ch = pcm->samples[0];
	right_ch = pcm->samples[1];


	write_samples(hand, (void *) (pcm->samples), nsamples);

	fprintf(stderr, "[%c] \r", cazzatine[cazcount++]);
	bprmv(bp, po, po->potail);

	return MAD_FLOW_CONTINUE;
}

/*
 * This is the error callback function. It is called whenever a decoding
 * error occurs. The error is indicated by stream->error; the list of
 * possible MAD_ERROR_* errors can be found in the mad.h (or
 * libmad/stream.h) header file.
 */

static
enum mad_flow maderror(void *data, struct mad_stream *stream, struct mad_frame *frame)
{
	struct buffer *buffer = data;

	//fprintf(stderr, "decoding error 0x%04x (%s) \n", stream->error, mad_stream_errorstr(stream));

	/* return MAD_FLOW_BREAK here to stop decoding (and propagate an error) */
	return MAD_FLOW_CONTINUE;
}

/*
 * This is the function called by main() above to perform all the
 * decoding. It instantiates a decoder object and configures it with the
 * input, output, and error callback functions above. A single call to
 * mad_decoder_run() continues until a callback function returns
 * MAD_FLOW_STOP (to stop decoding) or MAD_FLOW_BREAK (to stop decoding and
 * signal an error).
 */


int decode(playout_buff * po, buffer_pool * bp, Sound_Handle hand)
{
	struct buffer *buffer = calloc(1, sizeof(struct buffer));
	struct mad_decoder decoder;
	int result = 1;

	/* initialize our private message structure */
	buffer->po = po;
	buffer->bp = bp;
	buffer->hand = hand;

	/* configure input, output, and error functions */

	mad_decoder_init(&decoder, &buffer, madinput, 0 /* header */ , 0 /* filter */ , madoutput,
			 maderror, 0 /* message */ );

	/* start decoding */
	while (1) {

		if (po->potail == -1)
			continue;

		result = mad_decoder_run(&decoder, MAD_DECODER_MODE_ASYNC);
	}


	/* release the decoder */
	mad_decoder_finish(&decoder);

	return result;
}
