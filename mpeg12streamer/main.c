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


#include <config.h>

#include <time.h>

#include <netembryo/mpeg.h>
#include <sys/time.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
//#include <glib.h>
#include <getopt.h>


#define SRC_BUF_DIM 65535 
#define DST_BUF_DIM 1400 

#define MCAST_ADDR "224.124.0.1"
#define MCAST_PORT "1234"

typedef struct _RTP_header {
	/* byte 0 */
	#if (BYTE_ORDER == LITTLE_ENDIAN)
		unsigned char csrc_len:4;		/* expect 0 */
		unsigned char extension:1;		/* expect 1, see RTP_OP below */
    		unsigned char padding:1;		/* expect 0 */
    		unsigned char version:2;		/* expect 2 */
	#elif (BYTE_ORDER == BIG_ENDIAN)
    		unsigned char version:2;
    		unsigned char padding:1;
    		unsigned char extension:1;
    		unsigned char csrc_len:4;
	#else
    	#error Neither big nor little
	#endif
	/* byte 1 */
	#if (BYTE_ORDER == LITTLE_ENDIAN)
    	unsigned char payload:7;		/* RTP_PAYLOAD_RTSP */
    	unsigned char marker:1;			/* expect 1 */
	#elif (BYTE_ORDER == BIG_ENDIAN)
    	unsigned char marker:1;
    	unsigned char payload:7;
	#endif
	/* bytes 2, 3 */
	unsigned short seq_no;
	/* bytes 4-7 */
	unsigned int timestamp;
	/* bytes 8-11 */
	unsigned int ssrc;					/* stream number is used here. */
} RTP_header;

static void usage()
{
	fprintf(stderr, "\nmpeg12streamer \n\t[--port=multicast-port | -p multicast-port (port)] \
			\n\t[--addr=multicast_address | -a multicast_address (ip multicast address)] \
			\n\t[--filename=filename.mp3 | -f filename.mp3 (mp3 to stream)] \
			\n\t[-t | --tcp (rtp over tcp; default udp) ] \
			\n\t[-l | --loop (not set as default) ]\n\n");
			/*\n\t[-e | --ssl (use_ssl if enable)]\n\n");*/
	return;
}


#define PRINT_HEADER printf("\n\n \
			\t\t MPEG 1/2 Multicast Streamer (NEuTrino test program)\n \
			\t\t (LS){3} Project http://streaming.polito.it\n \
			\t\t Politecnico di Torino\n\n \
			\n")

int main(int argc, char **argv)
{

	MediaProperties *properties;
	double timestamp;
	void *private_data;
	uint32 dst_nbytes;
	uint32 dst_nbytes_init=DST_BUF_DIM;
	uint8 *dst;//[DST_BUF_DIM];
	uint32 src_nbytes=SRC_BUF_DIM;
	uint8 src[SRC_BUF_DIM];
	char filename[255]="\0";
	char port[6]="\0";
	char maddr[128]="\0";
	char loop = 0;
	int fd;
	int m_flag=0;
	int type=UDP;
	int res=0;
	void *message; /*final rtppkt*/
	Sock *s;
	int sock; /*socket descriptor*/
	RTP_header r; 
	unsigned int hdr_size=0;
        unsigned int ssrc=0;					/* stream number is used here. */
	unsigned short seq_no=1;
	mpv_data *mpv;	
	int oflag = O_RDONLY;
	unsigned long int sleep_time;
	struct timespec ts;
	uint32 control=0;
	int n;
	static const char short_options[] = "a:p:f:slt";
	int32_t nerr = 0;	/*number of error */

	PRINT_HEADER;

#ifdef HAVE_GETOPT_LONG
	static struct option long_options[] = {
		{"addr", 1, 0, 'a'},
		{"port", 1, 0, 'p'},
		{"filename",    1, 0, 'f'},
		{"ssl",   0, 0, 's'},
		{"tcp",   0, 0, 't'},
		{"loop",   0, 0, 'l'},
		{"help",   0, 0, '?'},
		{0, 0, 0, 0}
	};


	while ((n = getopt_long(argc, argv, short_options, long_options, NULL)) != -1)
#else
	while( (n=getopt(argc,argv,short_options)) != -1)
#endif
	{
		switch (n) {
			case 0:	/* Flag setting handled by getopt-long */
				break;
			case 'a': {
				strcpy(maddr,optarg);	
				break;
				}
			case 'p': {
				strcpy(port,optarg);	
				break;
				}
			case 'f': {
				strcpy(filename,optarg);	
				break;
				}
			case 's':
				m_flag |= USE_SSL;	
				break;
			case 't':
				type = TCP;	
				break;
			case 'l':
				loop = 1;	
				break;
			case ':':
				fprintf(stderr, "Missing parameter to option!");
				break;
			case '?':
				nerr++;
				break;
			default:
				nerr++;
			}
	}

	if(!strlen(port))
		strcpy(port,MCAST_PORT);
	if(!strlen(maddr))
		strcpy(maddr,MCAST_ADDR);
	
	if(!strlen(filename))
		nerr++;

	if (nerr > 0) {
		usage();
		return 0;
	}

	fd=open(filename, oflag);
	if(fd==-1) {	
		fprintf(stderr,"Impossible open file.\n");	
		return -1;
	}
	
	properties=calloc(1,sizeof(MediaProperties));

	res=init(properties,&private_data);

	if(res!=0) {
		fprintf(stderr,"Init fails.\n");
		uninit(private_data);
		free(properties);
		return -1;
	}

	Sock_init();
	s = Sock_connect(maddr, port, &sock, type,m_flag);
	if(!s) {
		fprintf(stderr,"Sock_connect returns NULL\n");
		free(properties);
		return -1;
	
	}

	message=calloc(1,DST_BUF_DIM + 12);
	properties->payload_type =32; /*MPV*/

	if(type==TCP) {
		dst_nbytes_init=SRC_BUF_DIM+8;
		dst=calloc(1,SRC_BUF_DIM+8);
	}
	else
		dst=calloc(1,DST_BUF_DIM);

	fprintf(stderr, "[NET] address: %s; port: %s; \n",maddr,port);
stream:
	do {
		res = get_frame2(src, SRC_BUF_DIM, &timestamp, fd, properties, private_data);
		
		if(res>0) {
			control=0;
			int retpkt=0;
			src_nbytes=res;
			mpv = (mpv_data *)private_data;
			do {	
				dst_nbytes=dst_nbytes_init;
				retpkt=packetize(dst, &dst_nbytes, src+control, src_nbytes-control, properties, private_data);
				if(retpkt>0)
					control+=retpkt; 
				else
					break;
				/*add rtp header*/
				memset(message,0,DST_BUF_DIM + 12);
		    		hdr_size=sizeof(r);	
				r.version = 2;
    				r.padding = 0;
				r.extension = 0;
   				r.csrc_len = 0;
				r.marker = (control==src_nbytes)?1:0;//!(mpv->is_fragmented);
		    		r.payload = properties->payload_type;
				
				r.seq_no = htons((seq_no++)%65535);
		   		r.timestamp=htonl(timestamp);
				
				r.ssrc = htonl(ssrc);
				memset(message,0,sizeof(message));
				memcpy(message,&r,hdr_size);
				memcpy(message+hdr_size,dst,dst_nbytes);

				/*send pkt*/
				Sock_write(s, message, hdr_size+dst_nbytes);			
			} while(control<src_nbytes && retpkt>0);
			/*wait*/
			sleep_time = (800000000 / ((double) properties->frame_rate)) ;
			ts.tv_sec=0;
			ts.tv_nsec =sleep_time;
			nanosleep(&ts, NULL);
		}
		else
			fprintf(stderr,"res=%d\n",res);
	fprintf(stderr, "[MPV] bitrate: %d; framerate: %d; seconds: %2d; minutes: %2d; hours: %2d; timestamp: %f \r",properties->bit_rate,properties->frame_rate,mpv->seconds,mpv->minutes,mpv->hours,timestamp);
	} while(res!=ERR_EOF);

	if(loop) {
		lseek(fd,0, SEEK_SET);
		goto stream;
	}
	uninit(private_data);
	close(fd);
	Sock_close(s);
	
	return 0;
}
