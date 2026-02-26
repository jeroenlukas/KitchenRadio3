#include "configuration/krConfig.h"

#include <Arduino.h>
#include <AudioTools.h>
#include <AudioTools/AudioLibs/VS1053Stream.h>
#include <AudioTools/Communication/AudioHttp.h>


#include "information/krInformation.h"

// NOTE: do not use https urls! This will cause problems when reconnecting.

// https://github.com/andyvans/ultimate-radio/blob/main/src/AudioOut.cpp
//https://github.com/clogboy/EspWebradioPlayer/blob/main/AudioModule.cpp

//ICYStreamBuffered url(1024*512);  
ICYStreamBuffered url(1024*512);  
VS1053Stream vs1053; // final audio output
StreamCopy copier(vs1053, url, 1024); // copy url to decoder

void webradio_metadata_cb(MetaDataType type, const char* str, int len);
void webradio_handle();

void webradio_begin()
{

    // Setup VS1053
    auto cfg = vs1053.defaultConfig();
    cfg.is_encoded_data = true; // vs1053 is accepting encoded data
    // Use your custom pins or define in AudioCodnfig.h
    cfg.cs_pin = CONFIG_PIN_VS1053_CS; 
    cfg.dcs_pin = CONFIG_PIN_VS1053_DCS;
    cfg.dreq_pin = CONFIG_PIN_VS1053_DREQ;
    //cfg.reset_pin = VS1053_RESET;
    vs1053.begin(cfg);

    // Setup callback
    url.setMetadataCallback(webradio_metadata_cb);
    
    // Request metadata
    url.httpRequest().header().put("Icy-MetaData","1");    
}

void webradio_handle()
{
    if(information.audioPlayer.soundMode == WEBRADIO)
    {
        copier.copy();
    }
}

void webradio_connect(int station_idx)
{
    Serial.println("CONNECT");
    
    if(url.begin("http://samcloud.spacial.com/api/listen?sid=93462&m=sc&rid=168006","audio/mp3"))
    {
        Serial.println("OK");        
        information.audioPlayer.soundMode = WEBRADIO;  
        return;      
    }
    Serial.println("FAIL");
}

void webradio_disconnect()
{
    Serial.println("DISCONNECT");
    if(information.audioPlayer.soundMode == WEBRADIO)
    {
        url.end();
        
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