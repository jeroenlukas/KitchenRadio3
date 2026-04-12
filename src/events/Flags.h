#ifndef KR_FLAGS_H
#define KR_FLAGS_H


//#include "hmi/flagsFrontPanel.h"
//#include "flagsMain.h"

/*
This file contains event flags, which can be set and read throughout the system to track events.
*/

class Flags_t
{
    public:
    Flags_t()
    {
        Serial.println("Flags constructor!");            
    }

    struct flagsMain_t
    {
        bool updateLog;
        bool displayRedraw;
    };

    struct flagsTickers_t
    {
        bool passed1s;
        bool passed30min;
        bool passed1min;
        bool userinput;
    };

    struct flagsFrontpanel_t
    {
        bool encoder1ButtonPressed;
        bool encoder2ButtonPressed;
        bool encoder1TurnRight;
        bool encoder1TurnLeft;
        bool encoder2TurnRight;
        bool encoder2TurnLeft;

        bool buttonOffPressed;
        bool buttonRadioPressed;
        bool buttonBluetoothPressed;
        bool buttonSystemPressed;
        bool buttonAlarmPressed;
        bool buttonLampPressed;

        bool buttonOffLongPressed;
        bool buttonRadioLongPressed;
        bool buttonBluetoothLongPressed;
        bool buttonSystemLongPressed;
        bool buttonAlarmLongPressed;
        bool buttonLampLongPressed;

        bool buttonAnyPressed;
    };

    flagsMain_t main;
    flagsFrontpanel_t frontPanel; 
    flagsTickers_t tickers;

};

extern Flags_t flags;

#endif