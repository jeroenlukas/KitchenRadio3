#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>

#include "Menu.h"

// Three main menus: System, Alarm, Bluetooth

// Menus
Menu menuSettings("Settings");
Menu menuAlarm("Alarm");
Menu menuLamp("Lamp");

// Settings items
ValueItem brightness("Brightness", 5, 1,100);
ValueItem treble("Treble", 3, -5,10);
ValueItem bass("Bass", 4, 0,10);

// Alarm items
ValueItem alarmDummy("(dummy)",3,0,10);

// Lamp items
ValueItem lampHue("Hue", 0, 0, 100);

MenuManager menuMgr;

//menuId_t menuId = MENU_HOME;

void menu_begin()
{
  menuSettings.addItem(&brightness);
  menuSettings.addItem(&treble);
  menuSettings.addItem(&bass);

  menuAlarm.addItem(&alarmDummy);

  menuLamp.addItem(&lampHue);

  menuMgr.addMenu(&menuSettings);
  menuMgr.addMenu(&menuAlarm);
  menuMgr.addMenu(&menuLamp);
}
