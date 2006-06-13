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

#include <ao/ao.h>
#define CONVSIZE 4096

void *read_side(void *arg)
{
	playout_buff *po = ((Arg *)arg)->pb;
	buffer_pool *bp = ((Arg *)arg)->bp;
	struct timespec ts;
        int result=0;
        int16_t convbuffer[CONVSIZE];

	while(bp->flcount < DEFAULT_MAX_QUEUE && !((Arg *)arg)->thread_dead) {
		//fprintf(stderr,"buffer = %d\n",bp->flcount);
		ts.tv_sec=0;
		ts.tv_nsec = 26122 * DEFAULT_MIN_QUEUE * 1000;  //only to rescale the process
		nanosleep(&ts, NULL);
	}

	while(!((Arg *)arg)->thread_dead) {
            float **pcm;
            int samples;
	    
            if(vorbis_synthesis(&vb,&op)==0) /* test for success! */
	        vorbis_synthesis_blockin(&vd,&vb);

	    while((samples=vorbis_synthesis_pcmout(&vd,&pcm))>0){
	        int j;
		int clipflag=0;
		int bout=(samples<CONVSIZE?samples:CONVSIZE);
		
		/* convert floats to 16 bit signed ints (host order) and
		   interleave */
                for(j=0;j<bout;j++){
		    int16_t *ptr=convbuffer+j;
		    float  *mono=pcm[i];
		    for(i=0;i<vi.channels;i++){
                        int val=mono[i]*32767.f;
		        /* might as well guard against clipping */
		        if(val>32767) val=32767;
		        
                        if(val<-32768) val=-32768;

		        *ptr=val;
		        ptr+=vi.channels;
		    }
		}		
		
        	ao_play(ao_dev, convbuffer, 2*bout*vi.channels);
		
		vorbis_synthesis_read(&vd,bout); /* tell libvorbis how
						   many samples we
						   actually consumed */	    
	    }


		
	    if(bp->flcount <= 1) {	
	    //prefill	
    		while(bp->flcount < DEFAULT_MID_QUEUE) {
		//fprintf(stderr,"buffer = %d\n",bp->flcount);
    	            ts.tv_sec=0;
    		    ts.tv_nsec = 26122 * DEFAULT_MIN_QUEUE * 1000;
                    //only to rescale the process
		    nanosleep(&ts, NULL);
		}
	    }
	}
	
	ao_close(ao_dev);
	ao_shutdown();

	free(buffer);
	((Arg *)arg)->thread_dead = 1;
	
	return NULL;
}

