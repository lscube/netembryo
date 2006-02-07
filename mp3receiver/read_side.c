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
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h> //for ntohl

#include <nemesi/bufferpool.h>
#include <programs/mp3receiver.h>

#include "minimad.h" 

#if ENABLE_LIBAO
#include <ao/ao.h>
#else
#include <programs/sound.h>
#endif //ENABLE_LIBAO


#define RTPHEADERSIZE 12
#define RTPEXTENSIONSIZE 4
#define HEADERSIZE ( RTPHEADERSIZE + RTPEXTENSIONSIZE )

void *read_side(void *arg)
{
	playout_buff *po = ((Arg *)arg)->pb;
	buffer_pool *bp = ((Arg *)arg)->bp;
	struct timespec ts;
	//minimad
	struct buffer *buffer = calloc(1, sizeof(struct buffer));
	struct mad_decoder decoder;
	int result = 1;

#if ENABLE_LIBAO
	int ao_id=0;
	ao_sample_format format; //ao
	ao_option *options=NULL; //ao
	ao_device *ao_dev; //ao
	int sound_ao_in_use=0; //ao

	//audio init
	//TODO IN MAD_OUTPUT
	ao_initialize(); //ao
	if((ao_id=ao_default_driver_id())<0) {//ao
		fprintf(stderr,"ao_default_driver_id error using default oss\n");
		ao_id = ao_driver_id("oss");
	}

	if(!sound_ao_in_use) {
		//TODO
		format.channels=2;
		//++else format.channels=1;
		format.bits=16;
		//TODO
		format.rate=44100;
		sound_ao_in_use=1;
		ao_dev=ao_open_live(ao_id, &format, options);
		buffer->ao_dev = ao_dev;
	}
#else
	Sound_Handle hand=NULL;
	int direction = O_WRONLY;

	//audio init
	//TODO IN MAD_OUTPUT
	sound_init();
	set_sound_duplex(FULL_DUPLEX);
	set_sound_eight_bit(0);
	
	if(!sound_in_use()) {
		//TODO
		set_stereo_mode();
		//TODO
		set_speed(44100);
		hand = sound_open(direction);
		buffer->hand = hand;
	}
#endif	
	/* initialize our private message structure */
	buffer->po = po;
	buffer->bp = bp;
	mad_timer_reset(&(buffer->timer));
	mad_decoder_init(&decoder, buffer, madinput, madheader , 0 /* filter */ , madoutput,
			 0 /*error*/, 0 /* message */ );
	if(bp->flcount <= 1) {	
		//prefill	
		while(bp->flcount < DEFAULT_MIN_QUEUE) {
			//fprintf(stderr,"buffer = %d\n",bp->flcount);
			ts.tv_sec=0;
			ts.tv_nsec = 26122 * DEFAULT_MIN_QUEUE * 1000;  //only to rescale the process
			nanosleep(&ts, NULL);
		}
	}


	while(1)
		result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);
	
#if ENABLE_LIBAO
	ao_close(ao_dev);
	ao_shutdown();
#endif //ENABLE_LIBAO

	//minimad
	/* release the decoder */
	mad_decoder_finish(&decoder);

	return NULL;
}

