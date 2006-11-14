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
  
  $Id: sound.c 1.15 Sat, 11 Dec 1999 23:53:26 -0600 dreier $
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <programs/sound.h>
/*
#ifdef HAVE_LINUX_TELEPHONY_H
#include <linux/telephony.h>
#endif
*/
/*#ifdef HAVE_LINUX_IXJUSER_H
#include <linux/ixjuser.h>
#endif
*/
#include <linux/soundcard.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <glib.h>
/*
#if defined (HAVE_GSM_H)
#include <gsm.h>
#elif defined (HAVE_GSM_GSM_H)
#include <gsm/gsm.h>
#else
#error "GSM header file not found."
#endif
#include "gphone.h"
#include "gphone-lib.h"
*/
#include <errno.h>
#include <string.h>


struct Sound_Handle {
  int fd;
};

enum {
  RAW_FRAME_LENGTH = 160        /* samples in an uncompressed GSM frame */
};

static int Sound_Use_Count = 0;
static Duplex_Type Duplex;
static Sound_Device_Type Device;
//static int Ixj_Port;
static int Eight_Bit;
static int Sound_FD = -1;
static int Sound_Direction;
static int Mic_Mute = 0;

G_LOCK_DEFINE_STATIC(Sound_Access);

void
sound_init(void)
{
/*
#ifdef HAVE_LINUX_TELEPHONY_H
#ifdef HAVE_LINUX_IXJUSER_H
  set_sound_ixj_port(PORT_POTS);
#endif
#endif
*/
}

static Sound_Handle
sound_handle_new(void)
{
  Sound_Handle handle;

  handle = g_malloc(sizeof *handle);

  return handle;
}

static void
sound_handle_free(Sound_Handle handle)
{
  g_return_if_fail(handle != NULL);

  g_free(handle);
}

size_t
get_enc_frame_length(void)
{
  //return(sizeof(gsm_frame));
  return 1; //TODO chicco
}

size_t
get_raw_frame_length(void)
{
  //return(RAW_FRAME_LENGTH * sizeof (gsm_signal));
  return 1; //TODO chicco
}

Duplex_Type
get_sound_duplex(void)
{
  Duplex_Type ret;

  G_LOCK(Sound_Access);
  ret = Duplex;
  G_UNLOCK(Sound_Access);

  return ret;
}

void
set_sound_duplex(Duplex_Type dup)
{
  if (!sound_in_use()) {
    G_LOCK(Sound_Access);
    Duplex = dup;
    G_UNLOCK(Sound_Access);
  }
}

Sound_Device_Type
get_sound_device(void)
{
  Sound_Device_Type ret;

  G_LOCK(Sound_Access);
  ret = Device;
  G_UNLOCK(Sound_Access);

  return ret;
}

void
set_sound_device(Sound_Device_Type dev)
{
  if (!sound_in_use()) {
    G_LOCK(Sound_Access);
    Device = dev;
    G_UNLOCK(Sound_Access);
  }
}
/*
int
get_sound_ixj_port(void)
{
  int ret;

  G_LOCK(Sound_Access);
  ret = Ixj_Port;
  G_UNLOCK(Sound_Access);

  return ret;
}

void
set_sound_ixj_port(int port)
{
  if (!sound_in_use()) {
    G_LOCK(Sound_Access);
    Ixj_Port = port;
    G_UNLOCK(Sound_Access);
  }
}
*/
void
set_sound_eight_bit(int eight)
{
  if (!sound_in_use()) {
    G_LOCK(Sound_Access);
    Eight_Bit = eight;
    G_UNLOCK(Sound_Access);
  }
}

void
sound_mute_mic(int mute)
{
  G_LOCK(Sound_Access);
  Mic_Mute = mute;
  G_UNLOCK(Sound_Access);
}

static int
sound_oss_open(int direction)
{
  int soundfd;
  int format, stereo, speed;

  soundfd = open("/dev/dsp", direction);
  if (soundfd == -1) {
    return soundfd;
  }

  if (!Eight_Bit) {
    format = AFMT_S16_LE;
  } else {
    format = AFMT_U8;
  }

  if (ioctl(soundfd, SNDCTL_DSP_SETFMT, &format) < 0) {
    //gphone_perror_exit("*** sound_open : SNDCTL_DSP_SETFMT", 2);
  }

  if ((Eight_Bit) && (format != AFMT_U8)) {
    //gphone_print_exit("*** sound_open : 8 bit unsigned samples not supported.\n", 2);
  }
  if ((!Eight_Bit) && (format != AFMT_S16_LE)) {
    //gphone_print_exit("*** sound_open : 16 bit signed samples not supported..\n", 2);
  }

  stereo = 0;                   /* mono */
  if (ioctl(soundfd, SNDCTL_DSP_STEREO, &stereo) < 0) {
   //gphone_perror_exit("*** sound_open : SNDCTL_DSP_STEREO", 2);
  }
  if (stereo != 0) {
    //gphone_print_exit("*** sound_open : mono not supported.\n", 2);
  }

  speed = 8000;
  if (ioctl(soundfd, SNDCTL_DSP_SPEED, &speed) < 0) {
    //gphone_perror_exit("*** sound_open : SNDCTL_DSP_SPEED", 2);
  }
  if (speed < 7950 || speed > 8050) {
    //gphone_print_exit("*** sound_open : 8khz sampling not supported.\n", 2);
  }

  return soundfd;
}

#if 0
static int
sound_ixj_open(int direction)
{
  int soundfd;
  int codec;

  soundfd = open("/dev/phone0", O_RDWR); /* ignore direction param */
  if (soundfd == -1) {
    return soundfd;
  }

#ifdef HAVE_LINUX_TELEPHONY_H
#ifdef HAVE_LINUX_IXJUSER_H
  if (!Eight_Bit) {
    codec = LINEAR16;
  } else {
    codec = LINEAR8;
  }

  ioctl(soundfd, IXJCTL_PORT, Ixj_Port);

  ioctl(soundfd, IXJCTL_AEC_START, AEC_HIGH);
  ioctl(soundfd, IXJCTL_DAA_AGAIN, AGRR06DB | AGX00DB);
  ioctl(soundfd, PHONE_REC_CODEC, codec);
  ioctl(soundfd, PHONE_PLAY_CODEC, codec);

  if (!ioctl(soundfd, PHONE_RING)) {
    g_warning("Cannot issue a RING to Quicknet/POTS device");
  }

  ioctl(soundfd, PHONE_REC_START);
  ioctl(soundfd, PHONE_PLAY_START);
#else
  g_warning("Compiled without Quicknet support and tried to open Quicknet device.");
#endif /* HAVE_LINUX_IXJUSER_H */
#else
  g_warning("Compiled without Quicknet support and tried to open Quicknet device.");
#endif /* HAVE_LINUX_TELEPHONY_H */

  return soundfd;
}
#endif // 0

static
int sound_open_device(int direction)
{
  switch (Device) {
  case OSS_DEVICE:
    return sound_oss_open(Sound_Direction);
    break;
  /*case IXJ_DEVICE:
    return sound_ixj_open(Sound_Direction);
    break;*/
  default:
    g_warning("Unknown sound device type: %d", Device);
    return -1;
    break;
  }
}

Sound_Handle
sound_open(int direction)
{
  int fd;
  Sound_Handle hand;

  G_LOCK(Sound_Access);

  if (Duplex == FULL_DUPLEX) {
    if (Sound_Use_Count == 0) {
      Sound_Direction = O_RDWR;
      Sound_FD = sound_open_device(Sound_Direction);
    }

    fd = Sound_FD;
  } else {
    if (Sound_Use_Count == 0) {
      Sound_Direction = direction;
      Sound_FD = sound_open_device(Sound_Direction);

      fd = Sound_FD;
    } else {
      if (direction == Sound_Direction) {
        fd = Sound_FD;
      } else {
        g_warning("attempt to open sound device in wrong direction");
        fd = -1;
      }
    }
  }
  
  if (fd == -1) {
    hand = NULL;
  } else {
    hand = sound_handle_new();
    hand -> fd = Sound_FD;
  }
  
  ++Sound_Use_Count;
  
  G_UNLOCK(Sound_Access);
  
  return hand;
}

int
sound_in_use(void)
{
  int in_use;

  G_LOCK(Sound_Access);
  in_use = Sound_Use_Count > 0;
  G_UNLOCK(Sound_Access);

  return in_use;
}

void
sound_close(Sound_Handle hand)
{
  g_return_if_fail(hand != NULL);

  G_LOCK(Sound_Access);

  --Sound_Use_Count;

  if (Sound_Use_Count == 0) {
    if (ioctl(hand -> fd, SNDCTL_DSP_SYNC, 0) < 0) {
      //gphone_perror_exit("*** sound_close : SNDCTL_DSP_SYNC", 2);
    }
    if (close(hand -> fd) < 0) {
      //gphone_perror_exit("*** sound_close : close soundfd", 2);
    }
  }

  sound_handle_free(hand);

  G_UNLOCK(Sound_Access);
}

void
read_samples(Sound_Handle hand, void *sample, int bufsize)
{
  int i;
  int readsize;
  int tot, bytes;
  gchar *readbuf;

  g_return_if_fail(hand != NULL);

  if (Mic_Mute) {
    for (i = 0; i < bufsize / 2; i++) {
      ((gint16 *) sample)[i] = 0;
    }
  } else {
    if (!Eight_Bit) {
      readsize = bufsize;
      readbuf = sample;
    } else {
      readsize = bufsize / 2;
      readbuf = g_malloc(readsize);
    }     

    tot = 0;
    while (tot < readsize) {
      bytes = read(hand -> fd, readbuf + tot, readsize - tot);

      if (bytes < 0) {
        //gphone_perror_exit("*** read_sample : read", 3);
      }

      tot += bytes;
    }

    if (Eight_Bit) {
      for (i = 0; i < readsize; i++) {
        ((char *) sample)[i * 2] = 0;
        ((char *) sample)[i * 2 + 1] = readbuf[i] - 0x80;
      }

      g_free(readbuf);
    }
  }
}

void
write_samples(Sound_Handle hand, void *sample, int bufsize)
{
  int i;
  int writesize;
  int tot, bytes;
  gchar *writebuf;

  g_return_if_fail(hand != NULL);

  if (!Eight_Bit) {
    writesize = bufsize;
    writebuf = (gchar *) sample;
  } else {
    writesize = bufsize / 2;

    writebuf = g_malloc(writesize / 2);

    for (i = 0; i < writesize; i++) {
      writebuf[i] = ((char *) sample)[i * 2 + 1] + 0x80;
    }
  }

  tot = 0;
  
  while (tot < writesize) {
    bytes = write(hand -> fd, writebuf + tot, writesize - tot);

    if (bytes < 0) {
      //gphone_perror_exit("*** write_sample : write", 3);
    }

    tot += bytes;
  }

  /* Does syncing here reduce latency?? */
  if (fsync(hand -> fd) > 0) {
      g_warning("cannot synch writing sample: %s", strerror(errno));
  }

  if (Eight_Bit) {
    g_free(writebuf);
  }
}

int
find_power_level(void *buf, int buflen)
{
  int i;
  guint32 level;

  level = 0;

  for (i = 0; i < buflen / 2; i++) {
    if (ABS(GINT16_FROM_LE(((gint16 *) buf)[i])) > level) {
      level = ABS(GINT16_FROM_LE(((gint16 *) buf)[i]));
    }
  }

  return (int) CLAMP(level, 0, G_MAXSHORT);
}


/*
 * Local variables:
 *  compile-command: "make -k libgphone.a"
 * End:
 */
