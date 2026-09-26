
#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <AudioLogger.h>
#include <AudioTools.h>
#include <AudioToolsConfig.h>
#include <AudioTools/AudioLibs/VS1053Stream.h>

#include "Audioplayer.h"

#include "../system/Logger.h"

#include "../system/Settings.h"
#include "../hmi/Frontpanel.h"

#include "Webradio.h"
#include "I2SReceiver.h"

VS1053Stream vs1053; // final audio output

// For test tone
AudioInfo info(44100, 2, 16);
SineWaveGenerator<int16_t> sineWave(30000);                // subclass of SoundGenerator with max amplitude of 32000
GeneratedSoundStream<int16_t> sound(sineWave);             // Stream generated from sine wave
StreamCopy Testtone_copier(vs1053, sound);  

void audioplayer_volume_set(int volume);
void audioplayer_pa_mute(bool mute);
void audioplayer_mode_set( soundMode_t mode);

void audioplayer_bass_set(int bass);
void audioplayer_treble_set(int treble);

uint8_t bt_wav_header[44] = 
{
    0x52, 0x49, 0x46, 0x46, // RIFF
    0xFF, 0xFF, 0xFF, 0xFF, // size
    0x57, 0x41, 0x56, 0x45, // WAVE
    0x66, 0x6d, 0x74, 0x20, // fmt
    0x10, 0x00, 0x00, 0x00, // subchunk1size
    0x01, 0x00,             // audio format - pcm
    0x02, 0x00,             // numof channels
    0x44, 0xac, 0x00, 0x00, //, //samplerate 44k1: 0x44, 0xac, 0x00, 0x00       48k: 48000: 0x80, 0xbb, 0x00, 0x00,
    0x10, 0xb1, 0x02, 0x00, //byterate
    0x04, 0x00,             // blockalign
    0x10, 0x00,             // bits per sample - 16
    0x64, 0x61, 0x74, 0x61, // subchunk3id -"data"
    0xFF, 0xFF, 0xFF, 0xFF  // subchunk3size (endless)
};

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

    // Set poweramp mute pin
    pinMode(CONFIG_PIN_PA_MUTE, OUTPUT);

    // Set soundmode to off
    audioplayer_mode_set(OFF);

    // Set bass/treble
    audioplayer_bass_set(settings.audio.tonecontrol.bass);
    audioplayer_treble_set(settings.audio.tonecontrol.treble);

    // Set volume
    audioplayer_volume_set(40);
}

// Set by system, when audiomode is set to 'off'
void audioplayer_pa_mute(bool mute)
{
    // Toggle power amp mute pin
    digitalWrite(CONFIG_PIN_PA_MUTE, !mute);
    LOGG_DEBUG("Set mute: " + String(mute));
}

// Set mute by user
void audioplayer_set_mute(bool mute)
{
    information.audioPlayer.mute = mute;
    audioplayer_pa_mute(mute);
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
        case TESTTONE:
            //LOGG_DEBUG("tone");
            Testtone_copier.copy();
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

void audioplayer_mode_set(soundMode_t mode)
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
        information.audioPlayer.bluetoothArtist = ""; 
        information.audioPlayer.bluetoothTitle = "";
    }
    else if(information.audioPlayer.soundMode == TESTTONE)
    {
        sineWave.end();
    }

    LOGG_INFO("Soft reset VS1053");

    vs1053.softReset();

    audioplayer_volume_set(information.audioPlayer.volume);
    audioplayer_bass_set(settings.audio.tonecontrol.bass);
    audioplayer_treble_set(settings.audio.tonecontrol.treble);

    // Begin the new sound mode
    switch(mode)
    {
        case WEBRADIO:
            if(webradio_connect(information.webRadio.station_index))
            {
                information.audioPlayer.soundMode = WEBRADIO;  
                audioplayer_pa_mute(false);
            }
            break;
        case BLUETOOTH:        
            information.audioPlayer.soundMode = BLUETOOTH;
            i2sreceiver_start();
            audioplayer_pa_mute(false);
            break;
        case TESTTONE:
            information.audioPlayer.soundMode = TESTTONE;
            audioplayer_pa_mute(false);            
            sound.begin();
            sineWave.begin(info, (float)information.audioPlayer.testToneFrequency);
            vs1053.write(bt_wav_header, 44); 

            break;
        case OFF:
            information.audioPlayer.soundMode = OFF;
            audioplayer_pa_mute(true);
            break;


    }

    frontpanel_leds_handle();
    information.audioPlayer.changing = false;

}

void audioplayer_bass_set(int bass)
{
    LOGG_DEBUG("Setting bass to "  + String(bass));
    settings.audio.tonecontrol.bass = bass;
    vs1053.setBassFrequencyLimit(settings.audio.tonecontrol.bass_freq);
    vs1053.setBass((float)bass / 100.0);
    LOGG_DEBUG("Setting real bass to "  + String((float)bass / 100.0));
}

void audioplayer_treble_set(int treble)
{
    LOGG_DEBUG("Setting treble to "  + String(treble));
    settings.audio.tonecontrol.treble = treble;
    vs1053.setTrebleFrequencyLimit(settings.audio.tonecontrol.treble_freq);
    vs1053.setTreble((float)treble / 100.0);
    LOGG_DEBUG("Setting real treble to "  + String((float)treble / 100.0));

}