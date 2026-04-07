#ifndef KR_AUDIOPLAYER_H
#define KR_AUDIOPLAYER_H

#include "../information/Information.h"
#include "../configuration/Config.h"

#include <Arduino.h>
#include <AudioLogger.h>
#include <AudioTools.h>
#include <AudioToolsConfig.h>
#include <AudioTools/AudioLibs/VS1053Stream.h>

extern void audioplayer_init();

extern void audioplayer_handle();

extern void audioplayer_volume_set(int volume);

extern void audioplayer_mode_set( soundMode_t mode);

extern VS1053Stream vs1053; // final audio output

#endif