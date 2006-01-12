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
#include <netembryo/wsocket.h>
#include <programs/rtp.h>
#include <programs/rtp-packet.h>
#include <programs/rtp-audio.h>

#include <programs/mp3streamer.h>
#include <sys/time.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <glib.h>
#include <getopt.h>


#define SRC_BUF_DIM 2048 
#define DST_BUF_DIM 2000 

#define MCAST_ADDR "224.124.0.1"
#define MCAST_PORT "1234"

static guint32 My_Ssrc=123456;
static guint16 My_Seq;
static guint32 Rtp_Packets_Sent;
static guint32 Rtp_Octets_Sent;

static void rtp_send(gchar *buf, int nbytes, rtp_payload_t pt, guint32 My_Timestamp, Sock *s, int marker)
{
	Rtp_Packet packet;

	packet = rtp_packet_new_allocate(nbytes, 0, 0);

	rtp_packet_set_csrc_count(packet, 0);
	rtp_packet_set_extension(packet, 0);
	rtp_packet_set_padding(packet, 0);
	rtp_packet_set_version(packet, RTP_VERSION);
	rtp_packet_set_payload_type(packet, pt);
	rtp_packet_set_marker(packet, marker);
	rtp_packet_set_ssrc(packet, My_Ssrc); 
	rtp_packet_set_seq(packet, My_Seq);
	rtp_packet_set_timestamp(packet, My_Timestamp);

	++My_Seq;

	g_memmove(rtp_packet_get_payload(packet), buf, nbytes);
	
	rtp_packet_send(packet, s, packet -> data_len);

	++Rtp_Packets_Sent;
	Rtp_Octets_Sent += rtp_packet_get_packet_len(packet);

	rtp_packet_free(packet);
}

static void usage()
{
	fprintf(stderr, "\nmp3streamer \n\t[--port=multicast-port | -p multicast-port (port)] \
			\n\t[--addr=multicast_address | -a multicast_address (ip multicast address)] \
			\n\t[--filename=filename.mp3 | -f filename.mp3 (mp3 to stream)] \
			\n\t[-t | --tcp (rtp over tcp; default udp) ] \
			\n\t[-l | --loop (not set as default) ]\n\n\r");
			/*\n\t[-e | --ssl (use_ssl if enable)]\n\n\r");*/
	return;
}


#define PRINT_HEADER printf("\n\n \
			\t MP3 Multicast Streamer (NEuTrino test program)\n \
			\t (LS){3} Project http://streaming.polito.it\n \
			\t Politecnico di Torino\n\n \
			\r")

int main(int argc, char **argv)
{

	MediaProperties *properties;
	double timestamp;
	void *private_data;
	uint32 dst_nbytes;
	uint32 dst_nbytes_init=DST_BUF_DIM;
	uint8 dst[DST_BUF_DIM];
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
	Sock *s;
	int sock; /*socket descriptor*/
	mpa_data *mpa;	
	int oflag = O_RDONLY;
	unsigned long int sleep_time;
	struct timespec ts;

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
		{"loop",   0, 0, 'l'},
		{"tcp",   0, 0, 't'},
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
			case 'l':
				loop = 1;	
				break;
			case 't':
				type = TCP;	
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
 	//oflag|=O_NONBLOCK;
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

	properties->payload_type = PAYLOAD_MPA; /*MPA*/

stream:
	do {
		res = get_frame2(src, SRC_BUF_DIM, &timestamp, fd, properties, private_data);
		if(res>0) {
			uint32 control=0;
			int retpkt=0;
			src_nbytes=res;
			mpa = (mpa_data *)private_data;
			do {	
				dst_nbytes=dst_nbytes_init;
				retpkt=packetize(dst, &dst_nbytes, src+control, src_nbytes-control, properties, private_data);
				if(retpkt>0)
					control+=retpkt;
				else
					break;
				rtp_send(dst, dst_nbytes, properties->payload_type, timestamp, s, mpa->fragmented);

			} while(control<=src_nbytes && retpkt>0);

			/*wait*/
			sleep_time=(double)mpa->frame_size/(double)properties->sample_rate * 950000000;//1000000000;
			ts.tv_sec=0;
			ts.tv_nsec =sleep_time;
			nanosleep(&ts, NULL);
		}
	fprintf(stderr, "[NET] address: %s; port: %s; \t [MPA] bitrate: %d; sample rate: %3.0f; time: %f \r",\
			maddr,		port,		properties->bit_rate, properties->sample_rate,timestamp);
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
