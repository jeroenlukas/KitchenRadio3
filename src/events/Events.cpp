#include "esp32-hal.h"
#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <WiFi.h>
#include "Flags.h"
#include "Events.h"

#include "../audio/Audioplayer.h"
#include "../audio/Webradio.h"
#include "../audio/I2SReceiver.h"
#include "../hmi/Display.h"
#include "../information/Weather.h"
#include "../hmi/Frontpanel.h"
#include "../information/Time.h"
#include "../system/Tickers.h"
#include "../hmi/Lamp.h"

#include "../hmi/Alarm.h"
#include "../system/Logger.h"


#include "../hmi/Menu.h"

void events_encoders();
void events_buttons();
void events_tickers();

void events_handle()
{
  events_encoders();
  events_buttons();
  events_tickers();

  if(flags.frontPanel.buttonAnyPressed)
  {
    flags.frontPanel.buttonAnyPressed = false;

    tickers_userinput_reset();
    
    //flags.main.displayRedraw = true;
    // Force a display refresh
    flags.tickers.displayrefresh = true;

    // Update frontpanel leds
    frontpanel_leds_handle();
  }
}

void events_tickers()
{
  // ------------------------------------------------
  // Interval timers

  // Execute every second
  if(flags.tickers.passed1s)
  {
    flags.tickers.passed1s = false;

    information.system.wifiRSSI = WiFi.RSSI();
    information.system.uptimeSeconds++;
    time_update();

    information.system.coreTemperature = (int)(temperatureRead());

    frontpanel_ldr_read();
    display_set_brightness_auto();  
    alarm_handle();  

    information.clock.colon_state = !information.clock.colon_state;
  }

  // Execute every minute
  if(flags.tickers.passed1min)
  {
    flags.tickers.passed1min = false;
    // ..
  }

  // Execute every half hour
  if(flags.tickers.passed30min)
  {
    flags.tickers.passed30min = false;
    weather_retrieve_40();
    weather_forecast_1h();
  }

  // Execute every hour
  if(flags.tickers.passed1h)
  {
    flags.tickers.passed1h = false;
  }

  if(flags.tickers.passed3h)
  {
    flags.tickers.passed3h = false;
  }

  if(flags.tickers.passed6h)
  {
    flags.tickers.passed6h = false;
    weather_forecast_1d();
  }

  if(flags.tickers.passed24h)
  {
    flags.tickers.passed24h = false;
  }

  // ------------------------------------------------
  // Misc timers

  // Refresh display, timer based or forced
  if(flags.tickers.displayrefresh)
  {
    flags.tickers.displayrefresh = false;
    display_draw();    
  }

  // No user input for x amount of time
  if(flags.tickers.userinput_menu)
  {
    flags.tickers.userinput_menu = false;
    if(menuMgr.isActive())
          menuMgr.exit();
  }

  // Close station select screen
  if(flags.tickers.userinput_stationselect)
  {
    flags.tickers.userinput_stationselect = false;
    information.webRadio.station_index_select = information.webRadio.station_index;
  }
}

void events_encoders()
{  
  // Change volume or value
  if(flags.frontPanel.encoder1TurnLeft)
  {
    flags.frontPanel.encoder1TurnLeft = false;

    if(menuMgr.isActive())
    {
      // Decrease value
      MenuItem* item = menuMgr.currentMenu()->getSelectedItem();      
      item->decrease();

    }
    else {
      if(information.audioPlayer.volume > 3) 
        audioplayer_volume_set(information.audioPlayer.volume - 3);
    }
  
  }


  if(flags.frontPanel.encoder1TurnRight)
  {
    flags.frontPanel.encoder1TurnRight = false;
    if(menuMgr.isActive())
    {
      // Increase value
      MenuItem* item = menuMgr.currentMenu()->getSelectedItem();
      item->increase();
    }
    else {    
      
      if(information.audioPlayer.volume < 100) 
        audioplayer_volume_set(information.audioPlayer.volume + 3);
    }
  }

  // Change channel or menu item
  if(flags.frontPanel.encoder2TurnLeft)
  {
    flags.frontPanel.encoder2TurnLeft = false;
    if(menuMgr.isActive())
    {
      menuMgr.prev();
    }
    else
    {
        
      if(information.audioPlayer.soundMode == WEBRADIO)
      {      
        if(information.webRadio.station_index_select > 0)
        {
          information.webRadio.station_index_select--;
        }
      }
    }
  }
  if(flags.frontPanel.encoder2TurnRight)
  {
    flags.frontPanel.encoder2TurnRight = false;
    
    if(menuMgr.isActive())
    {
      menuMgr.next();
      
    }
    else
    {
      if(information.audioPlayer.soundMode == WEBRADIO)
      {      
        if(information.webRadio.station_index_select < information.webRadio.station_count -1)
        {
          information.webRadio.station_index_select++;
        }
      }
    }
 
  }

}

void events_buttons()
{
  if(flags.frontPanel.buttonOffPressed)
  {
    flags.frontPanel.buttonOffPressed = false;
    if(menuMgr.isActive())
      menuMgr.back(); // Return to main menu or to parent menu
    else
      audioplayer_mode_set(OFF);

  }
  if(flags.frontPanel.buttonRadioPressed)
  {
    flags.frontPanel.buttonRadioPressed = false;
    audioplayer_mode_set(WEBRADIO);
  }
  if(flags.frontPanel.buttonBluetoothPressed)
  {
    flags.frontPanel.buttonBluetoothPressed = false;
    audioplayer_mode_set(BLUETOOTH);
  }

  if(flags.frontPanel.buttonSystemPressed)
  {
    flags.frontPanel.buttonSystemPressed = false;
    menuMgr.switchTo(MENU_SETTINGS);
    menuMgr.first();  // Always open with the first page
  }

  if(flags.frontPanel.buttonAlarmPressed)
  {
    flags.frontPanel.buttonAlarmPressed = false;

    // Pressing alarm button stops buzzing.
    if(information.alarm.state == ALARM_STATE_BUZZING)
    {
      alarm_stop();
    }
    else 
    {
      menuMgr.switchTo(MENU_ALARM);
      menuMgr.first();  // Always open with the first page
    }
  }

  if(flags.frontPanel.buttonLampPressed)
  {
    flags.frontPanel.buttonLampPressed = false;
    menuMgr.switchTo(MENU_LAMP);
    menuMgr.first();  // Always open with the first page
  }

  if(flags.frontPanel.buttonLampLongPressed)
  {
    flags.frontPanel.buttonLampLongPressed = false;
    lamp_toggle();
  }

  if(flags.frontPanel.encoder1ButtonPressed)
  {
    flags.frontPanel.encoder1ButtonPressed = false;
    // TODO mute
  }

  if(flags.frontPanel.encoder2ButtonPressed)
  {
    flags.frontPanel.encoder2ButtonPressed = false;

    if(menuMgr.isActive())
    {
      MenuItem* item = menuMgr.currentMenu()->getSelectedItem();
      if(item->getType() == MENU_ITEM)
      {
        LOGG_DEBUG("sub!!");
        
        menuMgr.enter();
      }
      else if(item->getType() == ACTION_ITEM)
      {
        LOGG_DEBUG("Action exec!");
        ActionItem* ai = (ActionItem*)item;
        ai->execute();
      }
    }
    else if(information.audioPlayer.soundMode == BLUETOOTH)
      i2sreceiver_playpause();

    else if(information.audioPlayer.soundMode == WEBRADIO)
    {      
      if(information.webRadio.station_index_select != information.webRadio.station_index)
      {
        // Switch to selected station
        webradio_disconnect();
        information.webRadio.station_index = information.webRadio.station_index_select;
        webradio_connect(information.webRadio.station_index);        
      }
    }
  }


}
