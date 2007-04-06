
/*inspired on gphone*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <programs/sound.h>
#include <linux/soundcard.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <glib.h>
#include <errno.h>
#include <string.h>


struct Sound_Handle {
	int fd;
};

enum {
	RAW_FRAME_LENGTH = 160
};

static int Sound_Use_Count = 0;
static Duplex_Type Duplex;
static Sound_Device_Type Device;
static int Eight_Bit;
static int Sound_FD = -1;
static int Sound_Direction;
static int Mic_Mute = 0;
static int stereo = 0, speed = 8000;

G_LOCK_DEFINE_STATIC(Sound_Access);

void sound_init(void)
{
}

static Sound_Handle sound_handle_new(void)
{
	Sound_Handle handle;

	handle = g_malloc(sizeof *handle);

	return handle;
}

static void sound_handle_free(Sound_Handle handle)
{
	g_return_if_fail(handle != NULL);

	g_free(handle);
}


Duplex_Type get_sound_duplex(void)
{
	Duplex_Type ret;

	G_LOCK(Sound_Access);
	ret = Duplex;
	G_UNLOCK(Sound_Access);

	return ret;
}

void set_sound_duplex(Duplex_Type dup)
{
	if (!sound_in_use()) {
		G_LOCK(Sound_Access);
		Duplex = dup;
		G_UNLOCK(Sound_Access);
	}
}

Sound_Device_Type get_sound_device(void)
{
	Sound_Device_Type ret;

	G_LOCK(Sound_Access);
	ret = Device;
	G_UNLOCK(Sound_Access);

	return ret;
}

void set_sound_device(Sound_Device_Type dev)
{
	if (!sound_in_use()) {
		G_LOCK(Sound_Access);
		Device = dev;
		G_UNLOCK(Sound_Access);
	}
}

void set_sound_eight_bit(int eight)
{
	if (!sound_in_use()) {
		G_LOCK(Sound_Access);
		Eight_Bit = eight;
		G_UNLOCK(Sound_Access);
	}
}

void sound_mute_mic(int mute)
{
	G_LOCK(Sound_Access);
	Mic_Mute = mute;
	G_UNLOCK(Sound_Access);
}

static int sound_oss_open(int direction)
{
	int soundfd;
	int format;
	//chicco
	int arg = 0x7fff0004; //http://www.opensound.com/pguide/index.html

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

	if (ioctl(soundfd, SNDCTL_DSP_STEREO, &stereo) < 0) {
		//gphone_perror_exit("*** sound_open : SNDCTL_DSP_STEREO", 2);
	}

	if (ioctl(soundfd, SNDCTL_DSP_SPEED, &speed) < 0) {
		//gphone_perror_exit("*** sound_open : SNDCTL_DSP_SPEED", 2);
	}

	//chicco
	if (ioctl(soundfd, SNDCTL_DSP_SETFRAGMENT, &arg)) { 
		//fprintf(stderr,"ioctl error for sound card fd\n");
	}


	return soundfd;
}


static int sound_open_device(int direction)
{
	switch (Device) {
	case OSS_DEVICE:
		return sound_oss_open(Sound_Direction);
		break;
	default:
		g_warning("Unknown sound device type: %d", Device);
		return -1;
		break;
	}
}

void set_stereo_mode()
{
	if (!sound_in_use()) {
		G_LOCK(Sound_Access);
		stereo = 1;	/* stereo */
		G_UNLOCK(Sound_Access);
	}
}

void set_speed(int s)
{
	if (!sound_in_use()) {
		G_LOCK(Sound_Access);
		speed = s;	/* stereo */
		G_UNLOCK(Sound_Access);
	}
}

Sound_Handle sound_open(int direction)
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
		hand->fd = Sound_FD;
	}

	++Sound_Use_Count;

	G_UNLOCK(Sound_Access);

	return hand;
}

int sound_in_use(void)
{
	int in_use;

	G_LOCK(Sound_Access);
	in_use = Sound_Use_Count > 0;
	G_UNLOCK(Sound_Access);

	return in_use;
}

void sound_close(Sound_Handle hand)
{
	g_return_if_fail(hand != NULL);

	G_LOCK(Sound_Access);

	--Sound_Use_Count;

	if (Sound_Use_Count == 0) {
		if (ioctl(hand->fd, SNDCTL_DSP_SYNC, 0) < 0) {
			//gphone_perror_exit("*** sound_close : SNDCTL_DSP_SYNC", 2);
		}
		if (close(hand->fd) < 0) {
			//gphone_perror_exit("*** sound_close : close soundfd", 2);
		}
	}

	sound_handle_free(hand);

	G_UNLOCK(Sound_Access);
}

void read_samples(Sound_Handle hand, void *sample, int bufsize)
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
			bytes = read(hand->fd, readbuf + tot, readsize - tot);

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

void write_samples(Sound_Handle hand, void *sample, int bufsize)
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
		bytes = write(hand->fd, writebuf + tot, writesize - tot);

		if (bytes < 0) {
			//gphone_perror_exit("*** write_sample : write", 3);
		}

		tot += bytes;
	}

	/* Does syncing here reduce latency?? */
	if (fsync(hand->fd) > 0) {
		g_warning("cannot synch writing sample: %s", strerror(errno));
	}

	if (Eight_Bit) {
		g_free(writebuf);
	}
}

int find_power_level(void *buf, int buflen)
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
