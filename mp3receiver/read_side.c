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


void *read_side(void *arg)
{
	Thread_Queue queue;
	gpointer buffer;
	MySlot *buf;
	int ret;	
	size_t size,len;
	char out[MAX_BUFFER_OUT];
	Sound_Handle hand;
	int direction = O_WRONLY;
	struct mpstr mp;
	struct timespec ts;
	unsigned long int sleep_time;
	
	//BITRATE = tabsel_123[mp.fr.lsf][mp.fr.lay-1][mp.fr.bitrate_index]
	int tabsel_123[2][3][16] = {
		{ {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},
		{0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},
		{0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,} },

		{ {0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,} }	
	};

	long freqs[9] = { 44100, 48000, 32000,
                  22050, 24000, 16000 ,
                  11025 , 12000 , 8000 };


	queue = (Thread_Queue)arg;
	
	InitMP3(&mp);
	while(1) {
		if(thread_queue_length(queue) ) {
			buffer=thread_queue_head(queue);
			thread_queue_remove(queue);
			buf=(MySlot *)buffer;

			len =buf->len; 
			if(len <= 0)
				continue;
			ret = decodeMP3(&mp,(buf->buffer)+16,len-16,out,MAX_BUFFER_OUT,&size);
			if(!sound_in_use()) {
				if(mp.fr.stereo!=MPG_MD_MONO)
					set_stereo_mode();
				set_speed(freqs[mp.fr.sampling_frequency]);
				hand = sound_open(direction);
			}
			while(ret == MP3_OK) {
				write_samples(hand, (void *)out, size);
				ret = decodeMP3(&mp,NULL,0,out,MAX_BUFFER_OUT,&size);
			}

			
			sleep_time=24816326;/*(double)(mp.fr.framesize)/(double)(freqs[mp.fr.sampling_frequency]) * 10000000000;*/
			fprintf(stderr, "[MPA] bitrate: %d; sample rate: %ld buffer: %d - sleep: %ld\r", \
					tabsel_123[mp.fr.lsf][mp.fr.lay-1][mp.fr.bitrate_index]*1000, \
					freqs[mp.fr.sampling_frequency],\
					thread_queue_length(queue), sleep_time);
			//wait
			ts.tv_sec=0;
			ts.tv_nsec = sleep_time;
			nanosleep(&ts, NULL);
		}
		
	}

	return NULL;
}

