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
    
    flags.main.displayRedraw = true;
  }
}

void events_tickers()
{
   // A display redraw is forced when a user event happens for example pushing a button  
  if(flags.main.displayRedraw)
  {
    flags.main.displayRedraw = false;
    display_draw();
  }

  // Execute every second
  if(flags.tickers.passed1s)
  {
    flags.tickers.passed1s = false;

    information.system.wifiRSSI = WiFi.RSSI();
    information.system.uptimeSeconds++;
    time_update();

    frontpanel_ldr_read();
    display_set_brightness_auto();    
  }

  if(flags.tickers.displayrefresh)
  {
    flags.tickers.displayrefresh = false;
    display_draw();
    
  }

  // Execute every minute
  if(flags.tickers.passed1min)
  {
    // ..
  }

    // Execute every half hour
  if(flags.tickers.passed30min)
  {
    flags.tickers.passed30min = false;
    weather_retrieve();
  }

  // No user input for x amount of time
  if(flags.tickers.userinput)
  {
    flags.tickers.userinput = false;
    if(menuMgr.isActive())
          menuMgr.exit();
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
      if (item->getType() == VALUE_ITEM) {
        ValueItem* val = (ValueItem*)item;  // safe now
        val->decrease();
      }
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
      if (item->getType() == VALUE_ITEM) {
        ValueItem* val = (ValueItem*)item;  // safe now
        val->increase();
      }
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
      //flags.main.displayRedraw = true;
    }
    else
    {
        
      if(information.audioPlayer.soundMode == WEBRADIO)
      {      
        if(information.webRadio.station_index > 0)
        {
          webradio_disconnect();
          information.webRadio.station_index--;
          webradio_connect(information.webRadio.station_index);
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
        if(information.webRadio.station_index < information.webRadio.station_count)
        {
          webradio_disconnect();
          information.webRadio.station_index++;
          webradio_connect(information.webRadio.station_index);
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
      menuMgr.exit();
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
    menuMgr.switchTo(MENU_ALARM);
  }

  if(flags.frontPanel.buttonLampPressed)
  {
    flags.frontPanel.buttonLampPressed = false;
    menuMgr.switchTo(MENU_LAMP);
  }

  if(flags.frontPanel.encoder1ButtonPressed)
  {
    flags.frontPanel.encoder1ButtonPressed = false;
    // TODO mute
  }

  if(flags.frontPanel.encoder2ButtonPressed)
  {
    flags.frontPanel.encoder2ButtonPressed = false;
    if(information.audioPlayer.soundMode == BLUETOOTH)
      i2sreceiver_playpause();
  }


}
