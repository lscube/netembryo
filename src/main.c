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

#include <netembryo/wsocket.h>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/time.h> 

//open write
#include <sys/stat.h>
#include <fcntl.h>


#define MODE_NONE   0
#define MODE_SERVER 1
#define MODE_CLIENT 2
#define DEFAULT_PORT "65000"

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

#define __exec(mode) static void exec_##mode (char * port)

int flag=0; /*SSL, MULTICAST etc etc*/
char filename[255];
int type=UDP;	
char ip[16];

__exec(server);
__exec(client);

static void usage()
{
	fprintf(stderr, "\nnetembryo --client | -c (client mode: sender) | --server | -s (server mode: receiver) \
			\n\t--port=my-server-port | -p my-server-port (port listen by the server) \
			\n\t--ip=ip-address | -i ip-address (IPv4, IPv6, unicast or multicast) \
			\n\t[--filename=outputfilename | -f outputfilename (file or named fifo or -. Default -)] \
			\n\t[--tcp | -t (default udp. If use ssl default is tcp)] \
			\n\t[-e | --ssl (use_ssl if enable)]\n\n");
	return;
}

int main(int argc, char **argv)
{

	static const char short_options[] = "r:p:f:i:cset";

	int n;
	int32_t nerr = 0;	/*number of error */
	unsigned int mode = MODE_NONE;
	char port[6];

	strcpy(port,DEFAULT_PORT);

#ifdef HAVE_GETOPT_LONG
	static struct option long_options[] = {
		{"server", 0, 0, 's'},
		{"client", 0, 0, 'c'},
		{"tcp", 0, 0, 't'},
		{"port",   1, 0, 'p'},
		{"filename",   1, 0, 'f'},
		{"ip",   1, 0, 'i'},
		{"ssl",    1, 0, 'e'},
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

			case 'c': {
				if (mode > 0) {
					fprintf(stderr, "client exor server!!!");
					nerr++;
				}
				mode = MODE_CLIENT;
				break;
				}
			case 's': {
				if (mode > 0) {
					fprintf(stderr, "client exor server!!!");
					nerr++;
				}
				mode = MODE_SERVER;
				break;
				}
			case 't': {
				type=TCP;
				break;
				}
			case 'p':
				strcpy(port,optarg);	
				break;
			case 'f':
				strcpy(filename,optarg);	
				break;
			case 'i':
				strcpy(ip,optarg);	
				break;
			case 'e':
				flag |= USE_SSL;	
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
	if (nerr > 0 || (mode == MODE_NONE)) {
		usage();
		return 0;
	}

	Sock_init();
	if (mode == MODE_SERVER)
		exec_server(port);
	else
		exec_client(port);
		
	return 0;
}

#define MAX_BUFFER_STR "65535"
#define MAX_BUFFER atoi(MAX_BUFFER_STR)


__exec(server)
{
	Sock *s, *accepted_s=NULL;
	int sock;
	char buffer[MAX_BUFFER];
	int dumpfd;
	int oflag=O_RDWR;
	RTP_header *rtp;
	
	if(flag & USE_SSL)
		type=TCP;

	if(type==TCP)
		s = Sock_bind(NULL, port, &sock, type, flag);
	else 
		s = Sock_bind(ip, port, &sock, type, flag);

	if(s == NULL) {
		printf("Sock_bind returns NULL\n");
		return;
	}		
	if(type == TCP)	{
		Sock_listen(s,10);
		accepted_s = Sock_accept(s);
		printf("Connection arrived\n");
		//Sock_close(s);
		if(accepted_s == NULL) {
			printf("Sock_accept returns NULL\n");
			return;
		}		
	}
	
	if(!strlen(filename) || strcmp(filename,"-")==0)
		dumpfd=1; //STDOUT
	else {
		oflag|=O_CREAT;
		dumpfd=open(filename,oflag);
		if(dumpfd==-1)
			printf("Mannaggia... not file for stdout\n");
	}

	printf("Reading...\n");
	memset(buffer,0,MAX_BUFFER);
	if(type == TCP) {
		/*Sock_read(accepted_s,(void *)buffer,MAX_BUFFER);
		//Sock_close(accepted_s); 
		//Sock_close(s);
		printf("\n%s\n",buffer);*/
		while(1) {
			int nx=0;
			nx=Sock_read(accepted_s,(void *)buffer,MAX_BUFFER);
			rtp=(RTP_header *)buffer;
			write(dumpfd,buffer+4+12,nx-4-12);
			//printf("message size=%d seq_no=%d\n",nx,ntohs(rtp->seq_no));
			if(nx<=0)
				break;
		}

	}
	else {
		while(1) {
			int nx=0;
			nx=Sock_read(s,(void *)buffer,MAX_BUFFER);
			rtp=(RTP_header *)buffer;
			write(dumpfd,buffer+4+12,nx-4-12);
			//printf("message size=%d seq_no=%d\n",nx,ntohs(rtp->seq_no));
			if(nx<=0)
				break;
		}
		close(dumpfd);
		
		Sock_close(s);
	}
		
	printf("done\n");
		
	return;
}

#define STDIN 0  /* file descriptor for standard input */
__exec(client) 
{
	char message[MAX_BUFFER];
	Sock *s;
	int sock; /*socket descriptor*/


	if(flag & USE_SSL)
		type=TCP;
	
	memset(message,0,sizeof(message));
	printf("Your message(max "MAX_BUFFER_STR" char): ");
	fflush(stdout);
	read(STDIN,message,MAX_BUFFER);	
	
	s = Sock_connect(ip, port, &sock, type,flag);
	if(!s) {
		printf("Sock_connect returns NULL\n");
		return;
	
		}
	Sock_write(s, (void *)message, strlen(message));
	Sock_close(s);

	return;
}
#undef STDIN
#undef MAX_BUFFER
#undef MAX_BUFFER_STR

