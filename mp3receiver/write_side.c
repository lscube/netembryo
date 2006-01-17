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
#include <sys/types.h>
#include <pthread.h>

#include <netembryo/wsocket.h>
#include <programs/thread-queue.h>
#include <programs/mp3receiver.h>

void *write_side(void *arg)
{
	Sock *s;
	MySlot *buf;
	int flag=0; /*SSL, MULTICAST etc etc*/
	int type=UDP;	
	int sock;
	Thread_Queue queue;
	
	pthread_mutex_lock( &count_mutex );
	printf("Writer Thread is starting....\n");
	s = Sock_bind("224.124.0.1", "1234", &sock, type, flag);	

	if(s == NULL) {
		printf("Sock_bind returns NULL\n");
		pthread_mutex_unlock( &count_mutex );
		
		return NULL;
	}		

	queue = (Thread_Queue)arg;
	buf=calloc(1,sizeof(MySlot));
	while(1) {
		if((buf->len=Sock_read(s,(void *)(buf->buffer),MAX_BUFFER))>0) {
			//printf("Writer Thread: pkt received len = %d\n",bufflen);
			thread_queue_add(queue, (gpointer)buf);	
		}
		else {
			printf("Writer Thread: error while reading\n");
			Sock_close(s);
			pthread_mutex_destroy( &count_mutex );
			
			return NULL;
		}
		if(thread_queue_length(queue)>=DEFAULT_MIN_QUEUE)
			pthread_mutex_unlock( &count_mutex );
	}

	pthread_mutex_destroy( &count_mutex);
	//Sock_close(s);
	return NULL;
}

