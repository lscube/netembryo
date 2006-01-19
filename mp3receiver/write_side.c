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
	Thread_Queue queue;
	int max_queue = ((Arg *)arg)->max_queue;
	int min_queue = ((Arg *)arg)->min_queue;

	queue = ((Arg *)arg)->queue;
	s = ((Arg *)arg)->sock;
	buf=calloc(1,sizeof(MySlot));
	while(1) {
		while(thread_queue_length(queue)> max_queue);
		//wait the reader
		
		if((buf->len=Sock_read(s,(void *)(buf->buffer),MAX_BUFFER))>0) {
			//printf("Writer Thread: pkt received len = %d\n",bufflen);
			thread_queue_add(queue, (gpointer)buf);	
		}
		else {
			printf("Writer Thread: error while reading\n");
			Sock_close(s);
			
			return NULL;
		}
	}

	//Sock_close(s);
	return NULL;
}

