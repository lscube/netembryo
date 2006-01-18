
/*inspired from gphone*/

#ifndef _SOUND_H
#define _SOUND_H 

#include <sys/types.h>

typedef enum {
	HALF_DUPLEX,
	FULL_DUPLEX
} Duplex_Type;

typedef enum {
	OSS_DEVICE,
} Sound_Device_Type;

typedef struct Sound_Handle *Sound_Handle;

void sound_init(void);
Duplex_Type get_sound_duplex(void);
void set_sound_duplex(Duplex_Type dup);
Sound_Device_Type get_sound_device(void);
void set_sound_device(Sound_Device_Type dev);
void set_sound_eight_bit(int eight);
void sound_mute_mic(int mute);
void set_stereo_mode(); //default mono
void set_speed(int s); //default 8000Hz
Sound_Handle sound_open(int direction);
int sound_in_use(void);
void sound_close(Sound_Handle hand);
void read_samples(Sound_Handle hand, void *sample, int bufsize);
void write_samples(Sound_Handle hand, void *sample, int bufsize);
int find_power_level(void *buf, int buflen);

#endif /* sound.h */
