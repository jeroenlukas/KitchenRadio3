
#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <AudioLogger.h>
#include <AudioTools.h>
#include <AudioToolsConfig.h>
#include <AudioTools/AudioLibs/VS1053Stream.h>

#include "../system/Logger.h"

#include "../system/Settings.h"


#include "Webradio.h"
#include "I2SReceiver.h"

VS1053Stream vs1053; // final audio output

void audioplayer_volume_set(int volume);

void audioplayer_init()
{
    LOGG_INFO("Audioplayer init");
    // Setup VS1053    
    auto cfg = vs1053.defaultConfig();
    cfg.is_encoded_data = true; // vs1053 is accepting encoded data
    
    cfg.cs_pin = CONFIG_PIN_VS1053_CS; 
    cfg.dcs_pin = CONFIG_PIN_VS1053_DCS;
    cfg.dreq_pin = CONFIG_PIN_VS1053_DREQ;
    cfg.reset_pin = -1;
    vs1053.begin(cfg);
    LOGG_INFO("Audioplayer started");

    // Set volume
    audioplayer_volume_set(40);
}

void audioplayer_handle()
{
    if(information.audioPlayer.changing)
    {
        return;
    }

    switch(information.audioPlayer.soundMode)
    {
        case OFF:
            break;
        case WEBRADIO:
            webradio_handle();
            break;
        case BLUETOOTH:
            i2sreceiver_handle();
            break;
        default:
            break;
    }
}

// Set volume (0..100)
void audioplayer_volume_set(int volume)
{
    volume = constrain(volume, 1, 100);
    information.audioPlayer.volume = volume;
    
    // Convert to logarithmic scale
    float vol_log =  (2.5 * 20 * log10((float)volume))/100;
    
    vs1053.setVolume(vol_log);
}

void audioplayer_mode_set( soundMode_t mode)
{
    //LOGG_INFO("Set mode to " + String(mode));

    information.audioPlayer.changing = true;

    // Stop the current sound mode
    if(information.audioPlayer.soundMode == WEBRADIO)
    {
        webradio_disconnect();
        
    }
    else if(information.audioPlayer.soundMode == BLUETOOTH)
    {
        i2sreceiver_stop();
    }

    LOGG_INFO("Soft reset VS1053");

    vs1053.softReset();

    audioplayer_volume_set(information.audioPlayer.volume);

    // Begin the new sound mode
    switch(mode)
    {
        case WEBRADIO:
            if(webradio_connect(0))
                information.audioPlayer.soundMode = WEBRADIO;  
            break;
        case BLUETOOTH:        
            information.audioPlayer.soundMode = BLUETOOTH;
            i2sreceiver_start();
            break;
        case OFF:
            information.audioPlayer.soundMode = OFF;

            break;


    }

     information.audioPlayer.changing = false;

}

void audioplayer_bass_set(int bass)
{
    LOGG_DEBUG("Setting bass to "  + String(bass));
    settings.audio.tonecontrol.bass = bass;
}

void audioplayer_treble_set(int treble)
{
    LOGG_DEBUG("Setting treble to "  + String(treble));
    settings.audio.tonecontrol.treble = treble;
    vs1053.setTreble((float)treble / 100.0);
    LOGG_DEBUG("Setting real treble to "  + String((float)treble / 100.0));

}