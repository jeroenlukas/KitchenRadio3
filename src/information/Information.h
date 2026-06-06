#ifndef KR_INFORMATION_H
#define KR_INFORMATION_H

#include <Arduino.h>

enum soundMode_t { OFF, WEBRADIO, BLUETOOTH, CHANGING };
enum bluetoothMode_t { BT_OFF, BT_NOTCONNECTED, BT_CONNECTED, BT_CONNECTING, BT_DISCONNECTING, BT_PLAYING, BT_PAUSED, BT_STOPPED, BT_UNKNOWN };
enum lampEffectType_t { EFFECT_NONE, EFFECT_RAINBOW, EFFECT_DOUBLERAINBOW, EFFECT_PULSE, EFFECT_WHEEL, EFFECT_COUNT };
enum alarmMode_t { ALARM_KITCHEN, ALARM_WAKEUP};
enum alarmState_t { ALARM_STATE_OFF, ALARM_STATE_COUNTDOWN, ALARM_STATE_PAUSED, ALARM_STATE_BUZZING };
     
class Information_t {
    public:
        Information_t()
        {
            Serial.println("Information constructor!");            
            
            audioPlayer.soundMode = OFF;
            system.compilationDateTime = __DATE__ ;

            alarm.mode = ALARM_KITCHEN;
            alarm.state = ALARM_STATE_OFF;
        }

        int hour;
        int minute;
        String timeShort;
        String dateMid;        

        struct System
        {
            //String deviceName;

            uint32_t uptimeSeconds;
            uint32_t bootTimeSeconds;
            
            int8_t wifiRSSI;            
            String ipAddress;

            uint8_t ldr;
            bool nightmode;

            int lastResetReason;

            String compilationDateTime;
        };
        struct AudioPlayer
        {
            enum soundMode_t soundMode;
            bool changing = false;

            int channels;
            int bitRate;
            int sampleRate;

            int volume;
            bool mute;

            int bass;
            int treble;

            bluetoothMode_t bluetoothMode;

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
            lampEffectType_t effect_type;
            int effect_speed;
        };

        struct Webradio
        {
            uint8_t station_index;
            String station_name;
            uint8_t station_count;
            uint8_t bufferPercentage;
            uint32_t bytesAvailable;
            uint32_t cntUnderruns;

            String metadataName;
            String metadataTitle;
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
        
        struct Alarm
        {
            alarmMode_t mode;
            alarmState_t state;
            int countdown_sec;
            String countdown_minsec;
        };


        System system;
        AudioPlayer audioPlayer;
        Lamp lamp;
        Webradio webRadio;
        Weather weather;
        Alarm alarm;

};

extern Information_t information;


#endif