#include <Arduino.h>

#include "../configuration/Config.h"
#include "../information/Information.h"

#include "Flags.h"
#include "Events.h"

#include "../audio/Audioplayer.h"

void events_handle()
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