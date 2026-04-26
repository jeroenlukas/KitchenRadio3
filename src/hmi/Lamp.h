#ifndef KR_LAMP_H
#define KR_LAMP_H

#include <Arduino.h>
#include "../information/Information.h"

extern void lamp_init();

extern void lamp_setstate(bool state);
extern void lamp_toggle();
extern void lamp_setcolor(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness);
extern void lamp_on();
extern void lamp_off();
extern void lamp_sethue(float hue);
extern void lamp_setsaturation(float saturation);
extern void lamp_setlightness(float lightness);
extern void lamp_seteffecttype(lampEffectType_t effect);
extern void lamp_seteffectspeed(float speed);


#endif