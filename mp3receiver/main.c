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
//#include <getopt.h>

#include <unistd.h>
//#include <sys/stat.h>
//#include <fcntl.h>
#include <string.h>
#include <pthread.h>


#include <programs/mp3receiver.h>
#include <programs/thread-queue.h>
#include <programs/sound.h>

static void usage()
{
	fprintf(stderr, "\nmp3receiver \
		\n\t--port=multicast-port | -p multicast-port (port) \
		\n\t--addr=multicast_address | -a multicast_address (ip multicast address) \
		\n\n\r");
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
	Thread_Queue queue;
	pthread_t write_buffer_thread, read_buffer_thread;
	int rtn;
  
	PRINT_HEADER;

	if (!g_thread_supported ()) 
		g_thread_init (NULL);
	else {
		fprintf(stderr,"gthread not supported \n");
		exit(1);
	}

	sound_init();
	if (Half_Flag) {
		set_sound_duplex(HALF_DUPLEX);
	} else {
		set_sound_duplex(FULL_DUPLEX);
	}
	set_sound_eight_bit(Eight_Flag);

	queue = thread_queue_new();
	
	if ((rtn = pthread_create(&write_buffer_thread, NULL, write_side,
                            (void *) queue))) {
		fprintf(stderr, "pthread_create talk_thread: %s\n", strerror(rtn));
		exit(1);
	}
	if ((rtn = pthread_create(&read_buffer_thread, NULL, read_side,
                            (void *) queue))) {
		fprintf(stderr, "pthread_create listen_thread: %s\n", strerror(rtn));
		exit(1);
	}

  	pthread_join(write_buffer_thread, NULL);
	pthread_join(read_buffer_thread, NULL);

	thread_queue_free(queue);

	return 0;
}
