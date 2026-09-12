#ifndef KR_SETTINGS_H
#define KR_SETTINGS_H

#include <Arduino.h>

class Settings_t {
    public:
        Settings_t()
        {
          Serial.println("Settings constructor!");            
          display.brightness_min = 10;
          display.brightness_max = 100;
        }

        String deviceName;
        String location;  // Example "Amsterdam,nl"

        struct Clock_t
        {
          String timezone;
        };

        struct Secrets_t
        {
          String wifi_ssid;
          String wifi_password;
          String openweathermap_api_key;
        };

        struct Audio_t
        {
          bool phase; // Whether speakers are in or out of phase
          struct Tonecontrol_t
          {
            int treble;
            int treble_freq;
            int bass;
            int bass_freq;
          };

          Tonecontrol_t tonecontrol;
        };

        struct Display_t
        {
          uint8_t brightness_min;
          uint8_t brightness_max;
        };
      
    Clock_t clock;
    Secrets_t secrets;
    Audio_t audio;
    Display_t display;
};

extern bool settings_load();

extern Settings_t settings;

#endif