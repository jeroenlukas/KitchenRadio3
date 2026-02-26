#ifndef KR_INFORMATION_H
#define KR_INFORMATION_H

#include <Arduino.h>

enum soundMode_t { OFF, WEBRADIO, BLUETOOTH};

class Information {
    public:
        Information()
        {
            Serial.println("Information constructor!");
            system.deviceName = "constructorTime!";
            webRadio.title = "";
            audioPlayer.soundMode = OFF;
        }

        int hour;
        int minute;
        String timeShort;
        String dateMid;

        

        struct System
        {
            String deviceName;

            uint32_t uptimeSeconds;
            int8_t wifiRSSI;
            
            String ipAddress;
            uint8_t ldr;

            int lastResetReason;
        };
        struct AudioPlayer
        {
            enum soundMode_t soundMode;

            int channels;
            int bitRate;
            int sampleRate;

            int volume;
            bool mute;

            int bass;
            int treble;

            int bluetoothMode;

            String bluetoothTitle;
            String bluetoothArtist;
            String bluetoothConnectionStateStr;
            int8_t bluetoothRSSI;
        };

        struct Lamp
        {
            bool state;
            float lightness;
            float hue; 
            float saturation;
            uint8_t effect_type;
            float effect_speed;
        };

        struct Webradio
        {
            uint8_t station_index;
            String station_name;
            uint8_t station_count;
            uint8_t buffer_pct;

            String title;
        };

        struct Weather
        {
            String stateShort;
            String stateLong;
            float temperature;
            double windSpeedKmh;
            int windSpeedBft;
            int stateCode;
            int pressure;
            float temperature_feelslike;
            int humidity;
            time_t sunrise;
            time_t sunset;
            String icon;
            String sunrise_str;
            String sunset_str;

        };
        


        System system;
        AudioPlayer audioPlayer;
        Lamp lamp;
        Webradio webRadio;
        Weather weather;

};

extern Information information;


#endif