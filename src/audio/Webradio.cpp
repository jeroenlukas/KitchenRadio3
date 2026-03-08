#include "../configuration/Config.h"

#include <Arduino.h>
#include <AudioTools.h>
#include <AudioTools/AudioLibs/VS1053Stream.h>
#include <AudioTools/Communication/AudioHttp.h>

#include "Audioplayer.h"

#include "../information/Information.h"

// NOTE: do not use https urls! This will cause problems when reconnecting.
// Examples:
// http://icecast.omroep.nl/radio1-bb-mp3
// http://22333.live.streamtheworld.com/KINK_DISTORTION.mp3

// https://github.com/andyvans/ultimate-radio/blob/main/src/AudioOut.cpp
//https://github.com/clogboy/EspWebradioPlayer/blob/main/AudioModule.cpp
//https://github.com/PeterDHabermehl/ESP32_bt_mono_speaker/blob/main/AudioTools_MonoSpeaker_rev2_fork.ino

ICYStreamBuffered streamUrl(1024*64);  //(1024*512);  
StreamCopy copier(vs1053, streamUrl, 1024); // copy url to decoder

String gUrl;

void webradio_metadata_cb(MetaDataType type, const char* str, int len);
void webradio_handle();

void webradio_url_set(String urlNew)
{
    Serial.println("Setting URL to " + urlNew);
    gUrl = urlNew;
}

void webradio_init()
{
    

    gUrl = "http://samcloud.spacial.com/api/listen?sid=93462&m=sc&rid=168006";

    // Setup callback
    streamUrl.setMetadataCallback(webradio_metadata_cb);
    
    // Request metadata
    streamUrl.httpRequest().header().put("Icy-MetaData","1");    
}

void webradio_handle()
{
    if(information.audioPlayer.soundMode == WEBRADIO)
    {
        copier.copy();
    }
}

bool webradio_connect(int station_idx)
{
    Serial.println("CONNECT");
    
    if(streamUrl.begin(gUrl.c_str(),"audio/mp3"))
    {
        Serial.println("OK");        
        
        return true;      
    }

    Serial.println("FAIL");
    return false;
}

void webradio_disconnect()
{
    Serial.println("DISCONNECT");
    if(information.audioPlayer.soundMode == WEBRADIO)
    {
        
        streamUrl.end();       
        streamUrl.flush();
        
        
        information.audioPlayer.soundMode = OFF;
    }
}

void webradio_metadata_cb(MetaDataType type, const char* str, int len)
{
  Serial.print("==> ");
  Serial.print(toStr(type));
  Serial.print(": ");
  Serial.println(str);

  information.webRadio.title = String(str);
}