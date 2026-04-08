#include "Information.h"
#include "../configuration/Config.h"

#include <Arduino.h>
#include <ezTime.h>

#include "Time.h"

#include "../system/Settings.h"
#include "../system/Logger.h"

Timezone tzLocal;
Timezone tzUTC;

void time_begin()
{
  LOGG_INFO("Using timezone " + settings.clock.timezone);
  tzLocal.setLocation(settings.clock.timezone.c_str());
  
}