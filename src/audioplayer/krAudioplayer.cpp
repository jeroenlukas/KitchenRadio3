#include "configuration/krConfig.h"
#include "information/krInformation.h"

#include <Arduino.h>
#include <AudioTools.h>
#include <AudioTools/AudioLibs/VS1053Stream.h>
#include <AudioTools/Communication/AudioHttp.h>
#include <AudioTools/Disk/AudioSourceURL.h>
#include "AudioTools/AudioCodecs/CodecCopy.h"

const char *urls[] = {
  "http://stream.srg-ssr.ch/m/rsj/mp3_128"
};

ICYStreamBuffered urlStream(1024);
VS1053Stream vs1053_out; // final audio output
AudioSourceURL source(urlStream, urls, "audio/mp3");
AudioPlayer player(source, vs1053_out, *new CopyDecoder());

void audioplayer_begin()
{
     Serial.println("!BEGIN!");
    // Setup VS1053
    auto cfg = vs1053_out.defaultConfig();
    cfg.is_encoded_data = true; // vs1053 is accepting encoded data
    // Use your custom pins or define in AudioCodnfig.h
    cfg.cs_pin = CONFIG_PIN_VS1053_CS; 
    cfg.dcs_pin = CONFIG_PIN_VS1053_DCS;
    cfg.dreq_pin = CONFIG_PIN_VS1053_DREQ;
    //cfg.reset_pin = VS1053_RESET;
    vs1053_out.begin(cfg);

    // Setup callback
    //..
//player.
    //player.begin();


}

void audioplayer_copy()
{
    player.copy();

}

void audioplayer_start()
{
    Serial.println("!START!");
    player.begin();
    information.audioPlayer.soundMode = WEBRADIO;
}

void audioplayer_stop()
{
    Serial.println("!STOP!");
    player.end();
    information.audioPlayer.soundMode = OFF;
}