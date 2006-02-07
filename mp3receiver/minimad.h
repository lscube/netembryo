#ifndef _MINIMAD_H_
#define _MINIMAD_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <mad.h>
#if ENABLE_LIBAO	
#include <ao/ao.h>
#endif //ENABLE_LIBAO

#include <nemesi/bufferpool.h>
#include <programs/sound.h>


/*
 * This is a private message structure. A generic pointer to this structure
 * is passed to each of the callback functions. Put here any data you need
 * to access from within the callbacks.
 */

struct buffer {
	playout_buff *po;
	buffer_pool *bp;
	Sound_Handle hand;
	mad_timer_t timer;
#if ENABLE_LIBAO	
	ao_device *ao_dev; //ao	
#endif //ENABLE_LIBAO
};

//int decode(playout_buff *po, buffer_pool *bp, Sound_Handle hand);
enum mad_flow madinput(void *data, struct mad_stream *stream);
enum mad_flow madoutput(void *data, struct mad_header const *header, struct mad_pcm *pcm);
enum mad_flow maderror(void *data, struct mad_stream *stream, struct mad_frame *frame);
enum mad_flow madheader(void *data, struct mad_header const *header);
#endif			
