/* * 
 *  $Id$
 *  
 *  This file is part of NetEmbryo 
 *
 *  NetEmbryo -- default network wrapper 
 *
 *  Copyright (C) 2005 by
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
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <netembryo/wsocket.h>
#include <programs/xiph_rtp.h>
#include <nemesi/bufferpool.h>

/* 
 * Takes a rtp packet from the input queue and puts vorbis 
 * frames in the playout queue
 */

void *write_side(void *arg)
{
	int n;
	int slot;	
	playout_buff *po = ((Arg *)arg)->pb;
	buffer_pool *bp = ((Arg *)arg)->bp;
	struct timespec ts;

	while(!((Arg *)arg)->thread_dead) {
		if( (slot=bpget(bp)) < 0) {
			//nms_printf(NMSML_VERB, "No more space in Playout Buffer!"BLANK_LINE);
			Sock_close(((Arg *)arg)->sock);
			((Arg *)arg)->thread_dead = 1;

			return NULL;
		}

		if((n=vorbis_frame(((Arg *)arg)->sock,(&(bp->bufferpool[slot])),BP_SLOT_SIZE))>0) {
			switch ( poadd(po, slot, 0) ) { //XXX hardly possible
				case PKT_DUPLICATED:
                                        bpfree(bp, slot);
				return NULL;
				break;
				case PKT_MISORDERED:
				break;
				default:
				break;
			}
	
			(po->pobuff[slot]).pktlen = n;
		}
		else {
			printf("Writer Thread: error while reading\n");
			Sock_close(((Arg *)arg)->sock);
			((Arg *)arg)->thread_dead = 1;
			
			return NULL;
		}
		if(bp->flcount > DEFAULT_MAX_QUEUE - 1) {	
			//fprintf(stderr,"buffer = %d\n",bp->flcount);
			ts.tv_sec=0;
			ts.tv_nsec = 26122;
			nanosleep(&ts, NULL);
		}

	}
	
	((Arg *)arg)->thread_dead = 1;
	
	return NULL;
}

