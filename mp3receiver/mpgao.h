#ifndef _MPGAO_H_
#define _MPGAO_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <mad.h>

#include <nemesi/bufferpool.h>
#include <programs/sound.h>

int decode(playout_buff *po, buffer_pool *bp, Sound_Handle hand);

#endif			
