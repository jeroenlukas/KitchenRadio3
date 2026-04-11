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

String time_convert(uint32_t timeInSeconds)
{  
  uint8_t seconds = timeInSeconds % 60;
  timeInSeconds /= 60;
  uint8_t minutes = timeInSeconds % 60;
  timeInSeconds /= 60;
  uint8_t hours = timeInSeconds % 24;
  uint8_t days = timeInSeconds / 24;
  String res;
  if(days > 0) res.concat(String(days) + "d ");
  if(hours > 0) res.concat(String(hours) + "h");
  if(minutes > 0) res.concat(String(minutes) + "m");
  res.concat(String(seconds) + "s");
  return res;
}

void time_waitForSync()
{
  LOGG_INFO("Retrieving time");
  waitForSync(5);

  time_update();

  LOGG_DEBUG("Time: " + information.timeShort);
  LOGG_DEBUG("Date: " + information.dateMid);
  return;
}

// Should be called every minute or second?
void time_update()
{
    information.hour = tzLocal.hour(TIME_NOW, LOCAL_TIME);
    information.minute = tzLocal.minute(TIME_NOW, LOCAL_TIME);
    information.timeShort = tzLocal.dateTime("H:i"); //String(u8x8_u8toa(information.hour, 2)) + ":" + String(u8x8_u8toa(information.minute, 2));
    information.dateMid = tzLocal.dateTime("D j M");
}