#include <Arduino.h>
#include <RotaryEncoder.h>
#include <Adafruit_MCP23X17.h>

#include "../configuration/Config.h"
#include "../information/Information.h"

#include "../events/Flags.h"

RotaryEncoder encoder1(CONFIG_PIN_ROTARY1_A, CONFIG_PIN_ROTARY1_B, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoder2(CONFIG_PIN_ROTARY2_A, CONFIG_PIN_ROTARY2_B, RotaryEncoder::LatchMode::FOUR3);

void frontpanel_encoders_read();

// -----------------------------------------------------------------------

void frontpanel_begin()
{

}

void frontpanel_handle()
{
  frontpanel_encoders_read();
}


void frontpanel_encoders_read()
{
    static int pos1 = 0;
    encoder1.tick();

    int newPos1 = encoder1.getPosition();
    if (pos1 != newPos1)
    {
        if ((int)(encoder1.getDirection()) == -1)
        {
            flags.frontPanel.encoder1TurnRight = true;
            Serial.println("1 right");
        }
        else
        {
            flags.frontPanel.encoder1TurnLeft = true;            
            Serial.println("1 left");
        }
        pos1 = newPos1;
        flags.frontPanel.buttonAnyPressed = true;
    }

    static int pos2 = 0;
    encoder2.tick();

    int newPos2 = encoder2.getPosition();
    if (pos2 != newPos2)
    {
        if ((int)(encoder2.getDirection()) == 1)
        {
            flags.frontPanel.encoder2TurnRight = true;
            Serial.println("2 right");
        }
        else
        {
            flags.frontPanel.encoder2TurnLeft = true;           
            Serial.println("2 left");
        }
        flags.frontPanel.buttonAnyPressed = true;
        pos2 = newPos2;
    }
}
