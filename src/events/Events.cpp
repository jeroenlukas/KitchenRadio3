#include <Arduino.h>

#include "../configuration/Config.h"
#include "../information/Information.h"

#include "Flags.h"
#include "Events.h"

#include "../audio/Audioplayer.h"

void events_encoders();
void events_buttons();

void events_handle()
{
  events_encoders();
  events_buttons();
}

void events_encoders()
{  
  if(flags.frontPanel.encoder1TurnLeft)
  {
    flags.frontPanel.encoder1TurnLeft = false;
    if(information.audioPlayer.volume > 3) 
      audioplayer_volume_set(information.audioPlayer.volume - 3);
  }
  if(flags.frontPanel.encoder1TurnRight)
  {
    flags.frontPanel.encoder1TurnRight = false;
    if(information.audioPlayer.volume < 100) 
      audioplayer_volume_set(information.audioPlayer.volume + 3);
  }
}

void events_buttons()
{
  if(flags.frontPanel.buttonOffPressed)
  {
    flags.frontPanel.buttonOffPressed = false;
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
}
