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

/*
 * header file for MPEG1-2 video elementary stream
 */

#ifndef _MPEGH
#define _MPEGH

#include <config.h>

#include <sys/types.h>
#include <unistd.h>

#include <netembryo/wsocket.h>
	#ifndef min
	#define min(a,b) (a<b)?a:b
	#endif // min

	typedef struct {                                 /* MPEG video specific headers */
		unsigned int ffc:3;
        	unsigned int ffv:1;
        	unsigned int bfc:3;                      /* |MBZ:5|T:1|TR:10|AN:1|N:1|S:1|B:1|E:1|P:3|FBV:1|BFC:3|FFV:1|FFC:3| */
        	unsigned int fbv:1;
        	unsigned int p:3;
        	unsigned int e:1;
        	unsigned int b:1;
        	unsigned int s:1;
        	unsigned int n:1;
        	unsigned int an:1;
        	unsigned int tr:10;
        	unsigned int t:1;
        	unsigned int mbz:5;
	} video_spec_head1;

	typedef struct {
		unsigned int d:1;                        /* |X:1|E:1|F[0,0]:4|F[0,1]:4|F[1,0]:4|F[1,1]:4|DC:2|PS:2|T:1|P:1|C:1|Q:1|V:1|A:1|R:1|H:1|G:1|D:1| */
        	unsigned int g:1;
        	unsigned int h:1;
        	unsigned int r:1;
        	unsigned int a:1;
       	 	unsigned int v:1;
        	unsigned int q:1;
        	unsigned int c:1;
        	unsigned int p:1;
        	unsigned int t:1;
        	unsigned int ps:2;
        	unsigned int dc:2;
        	unsigned int f11:4;
        	unsigned int f10:4;
        	unsigned int f01:4;
        	unsigned int f00:4;
        	unsigned int e:1;
        	unsigned int x:1;
	} video_spec_head2;

	typedef enum {MPEG_1,MPEG_2,TO_PROBE} standard;
	/*attivo l'extension header,...se mpeg2 :-) */
	#define MPEG2VSHE 1 


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

	#include <inttypes.h>

	typedef uint8_t uint8;
	typedef uint16_t uint16;
	typedef uint32_t uint32;
	typedef uint64_t uint64;
	typedef int8_t int8;
	typedef int16_t	int16;
	typedef int32_t	int32;
	typedef int64_t	int64;



#define	InputStream int //fd 
#define istream_read(nbytes, buf, istream) read(istream, buf, nbytes)

typedef enum {mc_undefined=-1, mc_frame=0, mc_sample=1} MediaCodingType;
typedef enum {MP_audio, MP_video, MP_application, MP_data, MP_control} MediaType;

typedef struct { 
	int32_t bit_rate; /*average if VBR or -1 is not usefull*/
	MediaCodingType coding_type;
	uint32 payload_type;
	uint32 clock_rate;
	char encoding_name[11];
	MediaType media_type;
	// Video specific properties:
	uint32 frame_rate;
	// more specific video information
	uint32 FlagInterlaced;
	//short StereoMode;
	uint32 PixelWidth;
	uint32 PixelHeight;
	uint32 DisplayWidth;
	uint32 DisplayHeight;
	uint32 DisplayUnit;
	uint32 AspectRatio;	
	uint8 *ColorSpace;
	float GammaValue;
} MediaProperties;

typedef struct {
	const char *encoding_name; /*i.e. MPV, MPA ...*/
	const MediaType media_type;
} MediaParserInfo;

typedef struct _MPV_DATA{

	uint32 is_buffered;
	uint32 buffer;

	uint32 is_fragmented;

	video_spec_head1 *vsh1;
	video_spec_head2 *vsh2;

	uint32 hours;
	uint32 minutes;
	uint32 seconds;
	uint32 pictures;
	uint32 temp_ref; /* pictures'count mod 1024*/
	uint32 picture_coding_type;
	standard std;
}mpv_data;

typedef struct {
	InputStream istream; 
	uint8 *src;
	uint32 src_nbytes;
} mpv_input;

int init(MediaProperties *properties, void **private_data);
int get_frame2(uint8 *dst, uint32 dst_nbytes, double *timestamp, InputStream istream, MediaProperties *properties, void *private_data);
int packetize(uint8 *dst, uint32 *dst_nbytes, uint8 *src, uint32 src_nbytes, MediaProperties *properties, void *private_data);
int uninit(void *private_data);
int get_field( uint8 *d, uint32 bits, uint32 *offset );
#endif
