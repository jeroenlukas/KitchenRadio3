#ifndef KR_AUDIOPLAYER_H
#define KR_AUDIOPLAYER_H

#include <Arduino.h>
#include <AudioTools/AudioLibs/VS1053Stream.h>

void audioplayer_init();

void audioplayer_volume_set(int volume);

extern VS1053Stream vs1053; // final audio output

//void audioplayer_copy();
//void audioplayer_start();
//void audioplayer_stop();

#endif