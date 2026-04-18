#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>

#include "../audio/Audioplayer.h"
#include "../system/Settings.h"
#include "Display.h"

#include "Menu.h"

// Three main menus: System, Alarm, Lamp

// Menus
Menu menuSystem("System");
Menu menuAlarm("Alarm");
Menu menuLamp("Lamp");

// Settings items
ValueItem viTreble("Treble", &(settings.audio.tonecontrol.treble), 1,100);
ValueItem viBass("Bass", &(settings.audio.tonecontrol.bass), 1 ,100);
InfoItem iiSystem("System Info");
InfoItem iiSmiley("Smiley");
InfoItem iiWeather("Weather");

// Alarm items
int dummy;
ValueItem alarmDummy("(dummy)",&dummy,0,10);

// Lamp items
int dummy2;
ValueItem lampHue("Hue", &dummy2, 0, 100);

// The menu manager
MenuManager menuMgr;



void menu_begin()
{  
  // Settings menu
  menuSystem.addItem(&iiSystem);
  iiSystem.setOnShowCallback(display_draw_custominfo_system);

  menuSystem.addItem(&iiSmiley);
  iiSmiley.setOnShowCallback(display_draw_custominfo_smiley);

  menuSystem.addItem(&iiWeather);
  iiWeather.setOnShowCallback(display_draw_custominfo_weather);

  menuSystem.addItem(&viTreble);
  viTreble.increment = 5;
  viTreble.setCallback(audioplayer_treble_set);
  
  menuSystem.addItem(&viBass);
  viBass.setCallback(audioplayer_bass_set);

  // Alarm menu
  menuAlarm.addItem(&alarmDummy);

  // Lamp menu
  menuLamp.addItem(&lampHue);

  // Add the menus to the manager
  menuMgr.addMenu(&menuSystem);
  menuMgr.addMenu(&menuAlarm);
  menuMgr.addMenu(&menuLamp);
}
