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
#include <sys/types.h>
#include <pthread.h>

#include <netembryo/wsocket.h>
#include <programs/mp3receiver.h>
#include <nemesi/bufferpool.h>

void *write_side(void *arg)
{
	int n;
	int slot;	
	playout_buff *po = ((Arg *)arg)->pb;
	buffer_pool *bp = ((Arg *)arg)->bp;

	while(1) {
		if( (slot=bpget(bp)) < 0) {
			//nms_printf(NMSML_VERB, "No more space in Playout Buffer!"BLANK_LINE);
			Sock_close(((Arg *)arg)->sock);
			return NULL;
		}

		if((n=Sock_read(((Arg *)arg)->sock,(&(bp->bufferpool[slot])),BP_SLOT_SIZE))>0) {
			switch ( poadd(po, slot, 0) ) {
				case PKT_DUPLICATED:
					//nms_printf(NMSML_VERB, "WARNING: Duplicate pkt found... discarded\n");
					bpfree(bp, slot);
				return 0;
				break;
				case PKT_MISORDERED:
					//nms_printf(NMSML_VERB, "WARNING: Misordered pkt found... reordered\n");
				break;
				default:
				break;
			}
	
			(po->pobuff[slot]).pktlen = n;
		}
		else {
			printf("Writer Thread: error while reading\n");
			Sock_close(((Arg *)arg)->sock);
			
			return NULL;
		}
	}

	return NULL;
}

