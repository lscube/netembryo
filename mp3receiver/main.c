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
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <getopt.h>

#include <unistd.h>
//#include <sys/stat.h>
//#include <fcntl.h>
#include <string.h>
#include <pthread.h>


#include <programs/mp3receiver.h>
#include <nemesi/bufferpool.h>
#include <netembryo/wsocket.h>

static void usage()
{
	fprintf(stderr, "\nmp3receiver \
		\n\t--port=multicast-port | -p multicast-port (port) \
		\n\t--addr=multicast_address | -a multicast_address (ip multicast address) \
		\n\t[-t | --tcp (rtp over tcp; default udp) ] \
		\n\n\r");
		/*\n\t[-e | --ssl (use_ssl if enable)]\n\n\r");*/

		//\n\t[--minbuf=min_dim_playout_buffer | -m min_dim_playout_buffer] \
		//\n\t[--maxbuf=max_dim_playout_buffer | -M max_dim_playout_buffer] \
	
	return;
}


#define PRINT_HEADER printf("\n\n \
			\t MP3 Multicast Receiver (NEuTrino test program)\n \
			\t (LS){3} Project http://streaming.polito.it\n \
			\t Politecnico di Torino\n\n \
			\r")

int main(int argc, char **argv)
{
	pthread_t write_buffer_thread, read_buffer_thread;
	int rtn;
	int set_addr=0;
	int set_port=0;
	int n;
	static const char short_options[] = "a:p:m:M:st";
	int32_t nerr = 0;	/*number of error */
	//NETWORK OPTIONS
	int type=UDP;	
	int flag = 0;
	char port[6];
	char maddr[128];
	Sock *s;
	int sock;
	int min_queue=0, max_queue=0;
	Arg *threadarg;

#ifdef HAVE_GETOPT_LONG
	static struct option long_options[] = {
		{"addr", 1, 0, 'a'},
		{"port", 1, 0, 'p'},
		{"minbuf",    1, 0, 'm'},
		{"maxbuf",    1, 0, 'M'},
		{"ssl",   0, 0, 's'},
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
				set_addr=1;
				break;
				}
			case 'p': {
				strcpy(port,optarg);	
				set_port=1;
				break;
				}
			case 'm': {
				min_queue=atoi(optarg);
				break;
				}
			case 'M': {
				max_queue=atoi(optarg);
				break;
				}
			case 's':
				flag |= USE_SSL;	
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
	PRINT_HEADER;
	
	if(!min_queue) {
		if((max_queue < DEFAULT_MIN_QUEUE) && max_queue)
			min_queue = max_queue / 2;
		else
			min_queue = DEFAULT_MIN_QUEUE;
	}
	if(!max_queue)
		max_queue = min_queue * 2;

	if ((nerr > 0) || (!set_addr) || (!set_port) || min_queue > max_queue) {
		if(min_queue > max_queue)
			fprintf(stderr,"Invalid playout buffer dimensions\n");
		usage();
		exit(1);
	}

	threadarg=calloc(1,sizeof(Arg));
	threadarg->thread_dead = 0;
	threadarg->min_queue = min_queue;
	threadarg->max_queue = max_queue;

	Sock_init();

	s = Sock_bind(maddr, port, &sock, type, flag);	
	if(s == NULL) {
		//Sock_close(s);
		free(threadarg);
		printf("Sock_bind returns NULL\n");
		exit(1);
	}		
	threadarg->sock=s;

	if((threadarg->bp=calloc(1,sizeof(buffer_pool))) == NULL) {
		Sock_close(s);
		free(threadarg);
		printf("Memory error: it is impossible to allocate memory for bufferpool\n");
		exit(1);
	
	}
	if(bpinit(threadarg->bp)) {
		Sock_close(s);
		free(threadarg->bp);
		free(threadarg);
		printf("Memory error: it is impossible to allocate memory for bufferpool\n");
		exit(1);
	}
		
	if((threadarg->pb=calloc(1,sizeof(playout_buff))) == NULL) {
		bpkill(threadarg->bp);
		free(threadarg->bp);
		Sock_close(s);
		free(threadarg);
		printf("Memory error: it is impossible to allocate memory for playout buffer\n");
		exit(1);
	}

	if(poinit(threadarg->pb,threadarg->bp)) {
		bpkill(threadarg->bp);
		free(threadarg->bp);
		free(threadarg->pb);
		Sock_close(s);
		free(threadarg);
		exit(1);
	}

	if ((rtn = pthread_create(&write_buffer_thread, NULL, write_side,
                            (void *) threadarg))) {
		fprintf(stderr, "pthread_create talk_thread: %s\n", strerror(rtn));
		bpkill(threadarg->bp);
		free(threadarg->bp);
		free(threadarg->pb);
		Sock_close(s);
		free(threadarg);
		exit(1);
	}

	if ((rtn = pthread_create(&read_buffer_thread, NULL, read_side,
                            (void *) threadarg))) {
		fprintf(stderr, "pthread_create listen_thread: %s\n", strerror(rtn));
		bpkill(threadarg->bp);
		free(threadarg->bp);
		free(threadarg->pb);
		Sock_close(s);
		free(threadarg);
		exit(1);
	}

  	pthread_join(write_buffer_thread, NULL);
	pthread_join(read_buffer_thread, NULL);

	Sock_close(s);
	bpkill(threadarg->bp);
	free(threadarg->bp);
	free(threadarg->pb);
	free(threadarg);
	printf("\n\t MP3RECEIVER IS GOING TO SHUTDOWN ... \n");

	return 0;
}
