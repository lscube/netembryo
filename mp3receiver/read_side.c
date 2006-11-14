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
#include <../mpglib/mpglib.h>
#include <../mpglib/mpg123.h>
#include <ao/ao.h>


#define RTPHEADERSIZE 12
#define RTPEXTENSIONSIZE 4
#define HEADERSIZE ( RTPHEADERSIZE + RTPEXTENSIONSIZE )

void *read_side(void *arg)
{
	int ret;	
	size_t size;
	char out[MAX_BUFFER_OUT];
	char finalout[MAX_BUFFER];
	int finalsize = 0;
	struct mpstr mp;
	playout_buff *po = ((Arg *)arg)->pb;
	buffer_pool *bp = ((Arg *)arg)->bp;
	int ao_id=0;
	ao_sample_format format; //ao
	ao_option *options=NULL; //ao
	ao_device *ao_dev; //ao
	int sound_in_use=0; //ao

	struct timespec ts;
	double sleep_time=0;
	unsigned int frame_size;
	struct timeval now;
	double time1=0.0;
	double mnow;
	//double timestamp1=0.0;
	//double timestamp=0.0;

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

#if ENABLE_DEBUG
	char cazzatine[4] = { '\\' , '|' , '/' , '-'};
	unsigned short cazcount=0;
#endif //ENABLE_DEBUG

	InitMP3(&mp);
	ao_initialize(); //ao
	if((ao_id=ao_default_driver_id())<0) //ao
		fprintf(stderr,"ao_default_driver_id error\n");
#if ENABLE_DEBUG
	fprintf(stderr,"ao_id = %d\n",ao_id);
#endif //ENABLE_DEBUG
	do {
		
		if(bp->flcount <= 1) {	
			//prefill	
			while(bp->flcount < DEFAULT_MIN_QUEUE) {
				//fprintf(stderr,"buffer = %d\n",bp->flcount);
				ts.tv_sec=0;
				ts.tv_nsec = 26122 * DEFAULT_MIN_QUEUE * 1000;  //only to rescale the process
				nanosleep(&ts, NULL);
			}
		}

		ret = decodeMP3( &mp, (char *)(&(*po->bufferpool)[po->potail]) + HEADERSIZE, (po->pobuff[po->potail]).pktlen - HEADERSIZE, out, MAX_BUFFER_OUT, &size );
		finalsize=0;

		if(ret != MP3_OK)
			continue;

		//timestamp = (double)(ntohl(((rtp_pkt *)(&(*po->bufferpool)[po->potail]))->time) * 1000);

		bprmv(bp,po,po->potail);
		// packet len
	        if (mp.fr.lay == 1) // layer 1
			frame_size = 384;
		else // layer 2 or 3
			frame_size = 1152;
		
		if(!sound_in_use) {
			if(mp.fr.stereo!=MPG_MD_MONO)
				format.channels=2;
			else
				format.channels=1;
			format.bits=16;
			format.rate=freqs[mp.fr.sampling_frequency];
			sound_in_use=1;
			ao_dev=ao_open_live(ao_id, &format, options);
			
		}
	
		while(ret == MP3_OK) {
			if(finalsize+size > sizeof(finalout)) {
				ao_play(ao_dev, (void *)finalout, finalsize); 
				finalsize=0;
			}
			//ao_play(ao_dev, (void *)out, size); 
			memcpy(finalout+finalsize,out,size);
			finalsize+=size;
			ret = decodeMP3(&mp,NULL,0,out,MAX_BUFFER_OUT,&size);
		}
		if(finalsize > 0) 
			ao_play(ao_dev, (void *)finalout, finalsize); 

#if ENABLE_DEBUG
		 fprintf(stderr, "[MPA] bitrate: %d - sample rate: %ld - buffer: %d%% [%c] \r", \
				tabsel_123[mp.fr.lsf][mp.fr.lay-1][mp.fr.bitrate_index]*1000, \
				freqs[mp.fr.sampling_frequency], bp->flcount*100/BP_SLOT_NUM, \
				cazzatine[cazcount%4]);
		cazcount++;
#endif //ENABLE_DEBUG
		gettimeofday(&now,NULL);
		mnow=(double)now.tv_sec*1000+(double)now.tv_usec/1000;
		if(bp->flcount < (DEFAULT_MAX_QUEUE - 1)) { //wow fear of buffer overflow!! Don't sleep... 
			sleep_time=(double)(frame_size)/(double)(freqs[mp.fr.sampling_frequency]) * 1000 - 5;// 1000000000;
			while ((mnow - time1) < sleep_time /*(timestamp  - timestamp1)*/) {
				gettimeofday(&now,NULL);
				mnow=(double)now.tv_sec*1000+(double)now.tv_usec/1000;
				/*wait*/
				ts.tv_sec=0;
				//ts.tv_nsec =sleep_time;
				ts.tv_nsec =1;//26122;
				nanosleep(&ts, NULL);
			}
		}
		time1=mnow;
		//timestamp1=timestamp ;


	}while(ao_dev!=NULL && sound_in_use);
	
	fprintf(stderr,"ao_device error\n");
	//ao_close(ao_dev);
	ao_shutdown();

	return NULL;
}

