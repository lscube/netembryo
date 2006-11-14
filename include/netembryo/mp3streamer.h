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
 *  part of code is taken from NeMeSI source code
 * */

#ifndef __MP3STREAMER_H
#define __MP3STREAMER_H

#include <config.h>

#include <sys/types.h>
#include <unistd.h>

#include <netembryo/wsocket.h>

#define uint8 uint8_t
#define uint16 uint16_t
#define uint32 uint32_t


	#define ERR_NOERROR			 0
	#define ERR_GENERIC			-1	
	#define ERR_NOT_FOUND			-2
	#define ERR_PARSE			-3
	#define ERR_ALLOC			-4
	#define ERR_INPUT_PARAM			-5
	#define ERR_NOT_SD			-6
	#define ERR_UNSUPPORTED_PT		-7
	#define ERR_EOF				-8
	#define ERR_FATAL			-9
	#define ERR_CONNECTION_CLOSE		-10

#define MP_PKT_TOO_SMALL -101
#define MP_NOT_FULL_FRAME -102

#ifndef min
#define min(a,b) (a<b)?a:b
#endif //min
#define	InputStream int //fd 

#define istream_read(nbytes, buf, istream) read(istream, buf, nbytes)
typedef enum {mc_undefined=-1, mc_frame=0, mc_sample=1} MediaCodingType;

typedef enum {MP_audio, MP_video, MP_application, MP_data, MP_control} MediaType;

typedef struct {
	enum {MPA_MPEG_2_5=0, MPA_MPEG_RES, MPA_MPEG_2, MPA_MPEG_1} id;
	enum {MPA_LAYER_RES=0, MPA_LAYER_III, MPA_LAYER_II, MPA_LAYER_I} layer;
	uint32 frame_size;
	// double pkt_len;
	uint32 pkt_len;
	uint32 probed;
	double time;
	// fragmentation suuport:
	uint8 fragmented;
	uint8 *frag_src;
	uint32 frag_src_nbytes;
	uint32 frag_offset;
} mpa_data;

typedef struct {
	uint8 id[3];
	uint8 major;
	uint8 rev;
	uint8 flags;
	uint8 size[4];
	// not using extended header
} id3v2_hdr;

typedef struct {
	InputStream istream; 
	uint8 *src;
	uint32 src_nbytes;
} mpa_input;


typedef struct { 
	int32_t bit_rate; /*average if VBR or -1 is not usefull*/
	MediaCodingType coding_type;
	uint32 payload_type;
	uint32 clock_rate;
	char encoding_name[11];
	MediaType media_type;
	// Audio specific properties:
	float sample_rate;/*SamplingFrequency*/
	float OutputSamplingFrequency;
	short audio_channels;
	uint32 bit_per_sample;/*BitDepth*/
} MediaProperties;

typedef struct {
	const char *encoding_name; /*i.e. MPV, MPA ...*/
	const MediaType media_type;
} MediaParserInfo;


int init(MediaProperties *properties, void **private_data);
int get_frame2(uint8 *dst, uint32 dst_nbytes, double *timestamp, InputStream istream, MediaProperties *properties, void *private_data);
int packetize(uint8 *dst, uint32 *dst_nbytes, uint8 *src, uint32 src_nbytes, MediaProperties *properties, void *private_data);
int uninit(void *private_data);
#endif
