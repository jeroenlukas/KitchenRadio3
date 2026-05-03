#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>

#include "../audio/Audioplayer.h"
#include "../system/Settings.h"
#include "Lamp.h"
#include "Display.h"

#include "Menu.h"

// Root menus
Menu menuSystem("System");
Menu menuAlarm("Alarm");
Menu menuLamp("Lamp");

// Settings items
IntItem viTreble("Treble", &(settings.audio.tonecontrol.treble), 1,100);
IntItem viBass("Bass", &(settings.audio.tonecontrol.bass), 1 ,100);
InfoItem iiSystem("System Info");
InfoItem iiSmiley("Smiley");
InfoItem iiWeather("Weather");

// Alarm items
int dummy;
IntItem alarmDummy("(dummy)",&dummy,0,10);

// Lamp items
BoolItem biLampState("State", &(information.lamp.state));
FloatItem fiHue("Hue", &(information.lamp.hue), 0.0, 1.0);
FloatItem fiBrightness("Brightness", &(information.lamp.lightness), 0.0, 0.5);
FloatItem fiSaturation("Saturation", &(information.lamp.saturation), 0.0, 1.0);

const char* oiEffectType_labels[] = { "None", "Rainbow", "Pulse", "Wheel"};
OptionItem oiEffectType("Effect Type", (int*)&(information.lamp.effect_type) , oiEffectType_labels, EFFECT_COUNT);
IntItem iEffectSpeed("Effect Speed", &(information.lamp.effect_speed), 1, 200);

// The menu manager
MenuManager menuMgr;

void onEffectChanged(int i)
{  
  lamp_seteffecttype((lampEffectType_t)i);
}

void menu_begin()
{  
  // === System menu ===
  menuSystem.addItem(&iiSystem);
  iiSystem.setOnShowCallback(display_draw_custominfo_system);

  menuSystem.addItem(&iiSmiley);
  iiSmiley.setOnShowCallback(display_draw_custominfo_smiley);

  menuSystem.addItem(&iiWeather);
  iiWeather.setOnShowCallback(display_draw_custominfo_weather);

  menuSystem.addItem(&viTreble);
  viTreble.increment = 5;
  viTreble.setCallback(audioplayer_treble_set);
  viTreble.wraparound = false;
  
  menuSystem.addItem(&viBass);
  viBass.setCallback(audioplayer_bass_set);
  viBass.wraparound = true;

  // === Alarm menu ===
  menuAlarm.addItem(&alarmDummy);

  // === Lamp menu ===
  menuLamp.addItem(&biLampState);
  biLampState.setCallback(lamp_setstate);
  
  menuLamp.addItem(&fiHue);  
  fiHue.setCallback(lamp_sethue);
  fiHue.increment = 0.01;
  fiHue.decimals = 3;
  fiHue.wraparound = true;
  
  
  menuLamp.addItem(&fiBrightness);
  fiBrightness.setCallback(lamp_setlightness);
  fiBrightness.increment = 0.01;

  menuLamp.addItem(&fiSaturation);
  fiSaturation.setCallback(lamp_setsaturation);
  fiSaturation.increment = 0.05;

  menuLamp.addItem(&oiEffectType);
  menuLamp.addItem(&iEffectSpeed);
  iEffectSpeed.increment = 10;
  iEffectSpeed.setCallback(lamp_seteffectspeed);
  oiEffectType.setCallback(onEffectChanged); 

  // Add the menus to the manager
  menuMgr.addMenu(&menuSystem);
  menuMgr.addMenu(&menuAlarm);
  menuMgr.addMenu(&menuLamp);
}
