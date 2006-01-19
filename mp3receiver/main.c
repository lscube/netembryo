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
#include <glib.h>
#include <getopt.h>

#include <unistd.h>
//#include <sys/stat.h>
//#include <fcntl.h>
#include <string.h>
#include <pthread.h>


#include <programs/mp3receiver.h>
#include <programs/thread-queue.h>
#include <programs/sound.h>
#include <netembryo/wsocket.h>

static void usage()
{
	fprintf(stderr, "\nmp3receiver \
		\n\t--port=multicast-port | -p multicast-port (port) \
		\n\t--addr=multicast_address | -a multicast_address (ip multicast address) \
		\n\t[--minbuf=min_dim_playout_buffer | -m min dim playout buffer] \
		\n\t[--maxbuf=max_dim_playout_buffer | -M max_dim_playout_buffer] \
		\n\t[-t | --tcp (rtp over tcp; default udp) ] \
		\n\n\r");
		/*\n\t[-e | --ssl (use_ssl if enable)]\n\n\r");*/
	
	return;
}


#define PRINT_HEADER printf("\n\n \
			\t MP3 Multicast Receiver (NEuTrino test program)\n \
			\t (LS){3} Project http://streaming.polito.it\n \
			\t Politecnico di Torino\n\n \
			\r")

static int Half_Flag = 0;
static int Eight_Flag = 0;

int main(int argc, char **argv)
{
	pthread_t write_buffer_thread, read_buffer_thread;
	int rtn;
	int set_addr=0;
	int set_port=0;
	int n;
	static const char short_options[] = "a:p:m:M:st";
	int32_t nerr = 0;	/*number of error */

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
	
	//NETWORK OPTIONS
	int type=UDP;	
	int flag;
	char port[6];
	char maddr[128];
	Sock *s;
	int sock;
	int min_queue=0, max_queue=0;
	Arg *threadarg;

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
	threadarg->min_queue = min_queue;
	threadarg->max_queue = max_queue;

	if (!g_thread_supported ()) 
		g_thread_init (NULL);//g_thread is used in buffer module
	else {
		fprintf(stderr,"gthread not supported \n");
		exit(1);
	}

	Sock_init();
	sound_init();
	if (Half_Flag) {
		set_sound_duplex(HALF_DUPLEX);
	} else {
		set_sound_duplex(FULL_DUPLEX);
	}
	set_sound_eight_bit(Eight_Flag);

	threadarg->queue = thread_queue_new();
	
	s = Sock_bind(maddr, port, &sock, type, flag);	
	if(s == NULL) {
		printf("Sock_bind returns NULL\n");
		exit(1);
	}		
	threadarg->sock=s;

	if ((rtn = pthread_create(&write_buffer_thread, NULL, write_side,
                            (void *) threadarg))) {
		fprintf(stderr, "pthread_create talk_thread: %s\n", strerror(rtn));
		exit(1);
	}
	while(thread_queue_length(threadarg->queue)<=threadarg->min_queue);
	//wait for prefilling

	if ((rtn = pthread_create(&read_buffer_thread, NULL, read_side,
                            (void *) threadarg))) {
		fprintf(stderr, "pthread_create listen_thread: %s\n", strerror(rtn));
		exit(1);
	}

  	pthread_join(write_buffer_thread, NULL);
	pthread_join(read_buffer_thread, NULL);

	thread_queue_free(threadarg->queue);
	free(threadarg);

	return 0;
}
