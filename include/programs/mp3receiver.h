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

#ifndef _MP3RECEIVERH
#define _MP3RECEIVERH

#include <config.h>
#include <sys/types.h>
#include <programs/thread-queue.h>
#include <netembryo/wsocket.h>

/*typedef struct WARG {
	Thread_Queue queue;
	Sock *sock;
};

typedef struct RARG {
	Thread_Queue queue;
};
*/

#include <pthread.h>

#define DEFAULT_MIN_QUEUE 10 
#define MAX_BUFFER_OUT 8192

#define MAX_BUFFER 16384 

typedef struct MYSLOT {
	char buffer[MAX_BUFFER];
	size_t len;
}MySlot;

void *write_side(void *arg);
void *read_side(void *arg);

#endif
