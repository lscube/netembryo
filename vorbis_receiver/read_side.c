/* * 
 *  $Id$
 *  
 *  This file is part of NetEmbryo 
 *
 *  NetEmbryo -- default network wrapper 
 *
 *  Copyright (C) 2006 by
 *  	
 *	- Federico Ridolfo	<federico.ridolfo@polito.it>
 *	- Luca Barbato          <lu_zero@gentoo.org>
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
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h> //for ntohl
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <nemesi/bufferpool.h>
#include <programs/xiph_rtp.h>
#include <vorbis/codec.h>
#include <netembryo/wsocket.h>
#include <programs/vorbis_receiver.h>


#if ENABLE_LIBAO
#include <ao/ao.h>
#else
#include <programs/sound.h>
#endif //ENABLE_LIBAO


void *read_side(void *arg)
{
	playout_buff *po = ((Arg *)arg)->pb;
	buffer_pool *bp = ((Arg *)arg)->bp;
	struct timespec ts;
        int result=0;

	while(bp->flcount < DEFAULT_MAX_QUEUE && !((Arg *)arg)->thread_dead) {
		//fprintf(stderr,"buffer = %d\n",bp->flcount);
		ts.tv_sec=0;
		ts.tv_nsec = 26122 * DEFAULT_MIN_QUEUE * 1000;  //only to rescale the process
		nanosleep(&ts, NULL);
	}

        

	while(result != -1 && !((Arg *)arg)->thread_dead) {

		result = 
		
		if(bp->flcount <= 1) {	
			//prefill	
			while(bp->flcount < DEFAULT_MID_QUEUE) {
				//fprintf(stderr,"buffer = %d\n",bp->flcount);
				ts.tv_sec=0;
				ts.tv_nsec = 26122 * DEFAULT_MIN_QUEUE * 1000;  //only to rescale the process
				nanosleep(&ts, NULL);
			}
		}
	}
	
#if ENABLE_LIBAO
	//ao_close(ao_dev);
	ao_shutdown();
#endif //ENABLE_LIBAO

	free(buffer);
	((Arg *)arg)->thread_dead = 1;
	
	return NULL;
}

