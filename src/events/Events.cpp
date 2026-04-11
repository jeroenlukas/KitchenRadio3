#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>

#include "Flags.h"
#include "Events.h"

#include "../audio/Audioplayer.h"
#include "../audio/Webradio.h"

#include "../system/Logger.h"


#include "../hmi/Menu.h"

void events_encoders();
void events_buttons();

void events_handle()
{
  events_encoders();
  events_buttons();
  /*if(flags.frontPanel.buttonAnyPressed)
  {
    flags.frontPanel.buttonAnyPressed = false;

  }*/

  if(flags.frontPanel.buttonAnyPressed)
  {
    flags.frontPanel.buttonAnyPressed = false;

    // TODO handle auto return to homescreen
    flags.main.displayRedraw = true;
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


}
