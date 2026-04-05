#ifndef KR_SETTINGS_H
#define KR_SETTINGS_H

#include <Arduino.h>

//enum soundMode_t { OFF, WEBRADIO, BLUETOOTH, CHANGING };
//enum bluetoothMode_t { BT_PLAYING, BT_PAUSED, BT_STOPPED };

class Settings_t {
    public:
        Settings_t()
        {
          Serial.println("Settings constructor!");            
        }

        String deviceName;
        String location;  // Example "Amsterdam,nl"

        struct Clock_t
        {
          String timezone;
        };

        struct Audio_t
        {
            struct Tonecontrol_t
            {
              int treble;
              int treble_freq;
              int bass;
              int bass_freq;
            };

            Tonecontrol_t tonecontrol;
        };
      
    Clock_t clock;
    Audio_t audio;
};

extern bool settings_load();

extern Settings_t settings;

#endif