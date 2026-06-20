#include <Arduino.h>
#include "../configuration/Config.h"
#include "../information/Information.h"
#include <Ticker.h>

#include "Alarm.h"
#include "../system/Logger.h"
#include "../events/Flags.h"
#include "Buzzer.h"
#include "Frontpanel.h"
#include "../audio/Audioplayer.h"

int volume_recall = 40;

void alarm_init()
{  

}

// Call every second
void alarm_handle()
{
  static int alarm_buzz_passed_s = 0;

  if(information.alarm.mode == ALARM_KITCHEN)
  {
    if(information.alarm.state == ALARM_STATE_COUNTDOWN)
    {
      frontpanel_leds_handle();
      information.alarm.countdown_sec--;

      alarm_buzz_passed_s = 0;

      int minutes = information.alarm.countdown_sec / 60;
      int seconds = information.alarm.countdown_sec % 60;

      String minsec = String(minutes) + ":";
      if (seconds < 10) {
          minsec += "0";
      }
      minsec += String(seconds);
      information.alarm.countdown_minsec = minsec;

      if(information.alarm.countdown_sec == 0)
      {
        LOGG_INFO("Alarm!");
        information.alarm.state = ALARM_STATE_BUZZING;
        volume_recall = information.audioPlayer.volume;
        audioplayer_volume_set(20);
      }
    }
    else if(information.alarm.state == ALARM_STATE_BUZZING)
    {
      buzzer_beep(500 * 1000); // Beep half a second
      alarm_buzz_passed_s++;
      
      // Automatically stop after x seconds
      if(alarm_buzz_passed_s > CONF_ALARM_BUZZ_STOP)
        alarm_stop();
    }
  }

}

void alarm_set(int sec)
{
  if(information.alarm.countdown_sec > 0)
  {
    information.alarm.state = ALARM_STATE_COUNTDOWN;
  }
}

void alarm_stop()
{
  information.alarm.state = ALARM_STATE_OFF;
  audioplayer_volume_set(volume_recall);
}