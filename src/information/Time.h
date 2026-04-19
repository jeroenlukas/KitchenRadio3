#ifndef KR_TIME_H
#define KR_TIME_H

#include <Arduino.h>
#include <ezTime.h>

extern Timezone tzLocal;

extern void time_begin();
extern void time_update();
extern void time_waitForSync();
extern String time_convert(uint32_t timeInSeconds);

#endif