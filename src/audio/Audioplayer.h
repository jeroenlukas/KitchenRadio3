#ifndef KR_AUDIOPLAYER_H
#define KR_AUDIOPLAYER_H

#include <Arduino.h>
#include <AudioTools/AudioLibs/VS1053Stream.h>

#include "../information/Information.h"

extern void audioplayer_init();

extern void audioplayer_handle();

extern void audioplayer_volume_set(int volume);

extern void audioplayer_mode_set( soundMode_t mode);

extern VS1053Stream vs1053; // final audio output

//void audioplayer_copy();
//void audioplayer_start();
//void audioplayer_stop();

#endif