/*
  Gnome-o-Phone - A program for internet telephony
  Copyright (C) 1999  Roland Dreier
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
  
  $Id: sound.h 1.6 Sat, 11 Dec 1999 23:53:26 -0600 dreier $
*/

#ifndef _SOUND_H
#define _SOUND_H 1

#include <sys/types.h>

typedef enum {
  HALF_DUPLEX,
  FULL_DUPLEX
} Duplex_Type;

typedef enum {
  OSS_DEVICE,
  //IXJ_DEVICE
} Sound_Device_Type;

typedef struct Sound_Handle *Sound_Handle;

void sound_init(void);
size_t get_enc_frame_length(void);
size_t get_raw_frame_length(void);
Duplex_Type get_sound_duplex(void);
void set_sound_duplex(Duplex_Type dup);
Sound_Device_Type get_sound_device(void);
void set_sound_device(Sound_Device_Type dev);
//int get_sound_ixj_port(void);
//void set_sound_ixj_port(int port);
void set_sound_eight_bit(int eight);
void sound_mute_mic(int mute);
Sound_Handle sound_open(int direction);
int sound_in_use(void);
void sound_close(Sound_Handle hand);
void read_samples(Sound_Handle hand, void *sample, int bufsize);
void write_samples(Sound_Handle hand, void *sample, int bufsize);
int find_power_level(void *buf, int buflen);

#endif /* sound.h */
