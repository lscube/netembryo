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
#include <glib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <programs/sound.h>
#include <programs/thread-queue.h>
#include <programs/mp3receiver.h>
#include <../mpglib/mpglib.h>
#include <../mpglib/mpg123.h>

struct mpstr mp;

void *read_side(void *arg)
{
	Thread_Queue queue;
	gpointer buffer;
	MySlot *buf;
	int ret;	
	size_t size,len;
	char out[MAX_BUFFER_OUT];
	//int dsp;
	//int flag = O_WRONLY;
	Sound_Handle hand;
	int direction = O_WRONLY;
		
	queue = (Thread_Queue)arg;
	
	//while(pthread_mutex_trylock( &count_mutex )==EBUSY);
	//pthread_mutex_unlock( &count_mutex );

	while(thread_queue_length(queue)<DEFAULT_MIN_QUEUE);

	//flag|=O_CREAT;
	//dsp = open("/home/chicco/work_in_progress/prova.mp3",flag);

	InitMP3(&mp);
	hand = sound_open(direction);
	while(1) {
		if(thread_queue_length(queue) ) {
			buffer=thread_queue_head(queue);
			thread_queue_remove(queue);
			buf=(MySlot *)buffer;

			len =buf->len; 
			//printf("Reader Thread: len = %d\n",len);
			if(len <= 0)
				continue;
			/*else
				if(write(dsp,(buf->buffer)+16,len-16)<=0) {
					printf("Reader Thread: [error]len = %d\n",len);
					break;
				}*/
			ret = decodeMP3(&mp,(buf->buffer)+16,len-16,out,MAX_BUFFER_OUT,&size);
			while(ret == MP3_OK) {
				//write(dsp,out,size);
				write_samples(hand, (void *)out, size);
				ret = decodeMP3(&mp,NULL,0,out,MAX_BUFFER_OUT,&size);
			}

		}
		
	}

	return NULL;
}

