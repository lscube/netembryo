/* *
 *  $Id$
 *
 *  This file is part of NetEmbryo
 *
 *  NetEmbryo -- default network wrapper
 *
 *  Copyright (C) 2006 by
 *
 *      - Federico Ridolfo      <federico.ridolfo@polito.it>
 *      - Luca Barbato          <lu_zero@gentoo.org>
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


#ifndef _VORBISRECEIVERH
#define _VORBISRECEIVERH

typedef struct ARG {
        buffer_pool      *bp;
        playout_buff     *pb;
        Sock             *sock;
        int              min_queue;
        int              max_queue;
        int              thread_dead;
        vorbis_info      vi; /* static vorbis bitstream settings */
        vorbis_comment   vc; /* user comments */
        vorbis_dsp_state vd; /* central packet->PCM decoder state */
        vorbis_block     vb; /* working space for packet->PCM decode */
} Arg;

#include <pthread.h>

#define DEFAULT_MIN_QUEUE ( (BP_SLOT_NUM < 10)?(BP_SLOT_NUM) : (BP_SLOT_NUM / 10) )
#define DEFAULT_MAX_QUEUE BP_SLOT_NUM
#define DEFAULT_MID_QUEUE ( DEFAULT_MAX_QUEUE / 2 )

#define MAX_BUFFER_OUT 8192

#define MAX_BUFFER ( 8192 * 4 )

int vorbis_frame(Sock *, void *buffer, int nbytes);

void *write_side(void *arg);
void *read_side(void *arg);


#endif

