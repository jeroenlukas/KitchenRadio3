#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>

#include "../audio/Audioplayer.h"
#include "../system/Settings.h"
#include "../information/Weather.h"
#include "Lamp.h"
#include "Display.h"
#include "Alarm.h"

#include "Menu.h"

// Root menus
Menu menuSystem("System");
Menu menuAlarm("Alarm");
Menu menuLamp("Lamp");

// [System]
InfoItem iiSmiley("Smiley");

// - [System] > [Audio]
Menu menuSystem_Audio("Audio");
IntItem viTreble("Treble", &(settings.audio.tonecontrol.treble), 1,100);
IntItem viBass("Bass", &(settings.audio.tonecontrol.bass), 1 ,100);
BoolItem biSpeakerPhase("Speaker phase", &(settings.audio.phase), "in phase", "out of phase");

// [System] > [Info]
Menu menuSystem_Overview("Info");
InfoItem iiSystemAdvanced("Advanced");

// [System] > [Weather]
Menu menuSystem_Weather("Weather");
InfoItem iiWeatherHourlyForecast("Hourly forecast");
InfoItem iiWeatherDailyForecast("Daily forecast");
ActionItem aiWeatherUpdate("Update weather");


// [Alarm]
MinSecItem msiCountDownAlarm("Alarm Time", &(information.alarm.countdown_sec), 1, 36000);

// [Lamp]
BoolItem biLampState("State", &(information.lamp.state));
FloatItem fiHue("Hue", &(information.lamp.hue), 0.0, 1.0);
FloatItem fiBrightness("Brightness", &(information.lamp.lightness), 0.0, 0.5);
FloatItem fiSaturation("Saturation", &(information.lamp.saturation), 0.0, 1.0);

// - [Lamp] > [Effects]
Menu menuLamp_Effects("Effects");
const char* oiEffectType_labels[] = { "None", "Rainbow", "Double Rainbow", "Pulse", "Wheel"};
OptionItem oiEffectType("Effect Type", (int*)&(information.lamp.effect_type) , oiEffectType_labels, EFFECT_COUNT);
IntItem iEffectSpeed("Effect Speed", &(information.lamp.effect_speed), 10, 500);

// The menu manager
MenuManager menuMgr;

void onEffectChanged(int i)
{  
  lamp_seteffecttype((lampEffectType_t)i);
}

void action_weather_retrieve()
{
  weather_retrieve_40();
  weather_forecast_1h();
}

void menu_begin()
{  
  // === System menu ===

  menuSystem.addItem(&menuSystem_Overview);
  menuSystem_Overview.setOnShowCallback(display_draw_systeminfo_overview);
  menuSystem_Overview.addItem(&iiSystemAdvanced);
  iiSystemAdvanced.setOnShowCallback(display_draw_systeminfo_advanced);

  // --- Weather submenu ---
  menuSystem.addItem(&menuSystem_Weather);
  menuSystem_Weather.setOnShowCallback(display_draw_custominfo_weather);
  menuSystem_Weather.addItem(&iiWeatherHourlyForecast);
  menuSystem_Weather.addItem(&iiWeatherDailyForecast);
  menuSystem_Weather.addItem(&aiWeatherUpdate);
  aiWeatherUpdate.setOnExecuteCallback(action_weather_retrieve);
  iiWeatherHourlyForecast.setOnShowCallback(display_draw_weather_forecast_hourly);
  iiWeatherDailyForecast.setOnShowCallback(display_draw_weather_forecast_daily);
  

  // --- Audio submenu ---
  menuSystem.addItem(&menuSystem_Audio);

  menuSystem_Audio.addItem(&viTreble);
  viTreble.increment = 5;
  viTreble.setCallback(audioplayer_treble_set);
  
  menuSystem_Audio.addItem(&viBass);
  viBass.increment = 5;
  viBass.setCallback(audioplayer_bass_set);

  menuSystem_Audio.addItem(&biSpeakerPhase);

  // --- Smiley ---
  menuSystem.addItem(&iiSmiley);
  iiSmiley.setOnShowCallback(display_draw_custominfo_smiley);


  // === Alarm menu ===
  menuAlarm.addItem(&msiCountDownAlarm);
  msiCountDownAlarm.setCallback(alarm_set);

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

  // Effects submenu
  menuLamp.addItem(&menuLamp_Effects);
  
  menuLamp_Effects.addItem(&oiEffectType);
  menuLamp_Effects.addItem(&iEffectSpeed);
  iEffectSpeed.increment = 10;
  iEffectSpeed.setCallback(lamp_seteffectspeed);
  oiEffectType.setCallback(onEffectChanged); 

  // Add the menus to the manager
  menuMgr.addMenu(&menuSystem);
  menuMgr.addMenu(&menuAlarm);
  menuMgr.addMenu(&menuLamp);
}
