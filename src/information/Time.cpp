#include "Information.h"
#include "../configuration/Config.h"

#include <Arduino.h>
#include <ezTime.h>

#include "Time.h"

#include "../system/Settings.h"
#include "../system/Logger.h"

Timezone tzLocal;
Timezone tzUTC;

void time_update();
void time_waitForSync();

void time_begin()
{
  LOGG_INFO("Using timezone " + settings.clock.timezone);
  tzLocal.setLocation(settings.clock.timezone.c_str());  
}

void time_waitForSync()
{
  LOGG_INFO("Retrieving time");
  waitForSync(5);

  time_update();
  return;
}

// Should be called every minute or second?
void time_update()
{
    information.hour = tzLocal.hour(TIME_NOW, LOCAL_TIME);
    information.minute = tzLocal.minute(TIME_NOW, LOCAL_TIME);
    information.timeShort = tzLocal.dateTime("H:i"); //String(u8x8_u8toa(information.hour, 2)) + ":" + String(u8x8_u8toa(information.minute, 2));
    information.dateMid = tzLocal.dateTime("D j M");
    LOGG_DEBUG("Time: " + information.timeShort);
    LOGG_DEBUG("Date: " + information.dateMid);

}