#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <AudioTools.h>
#include <AudioTools/AudioLibs/VS1053Stream.h>
#include <AudioTools/Communication/AudioHttp.h>
#include <AdvancedLogger.h>

#include "cbuf_ps.h"
#include "Audioplayer.h"


#include "../system/Stations.h"

// NOTE: do not use https urls! This will cause problems when reconnecting.
// Examples:
// http://icecast.omroep.nl/radio1-bb-mp3
// http://22333.live.streamtheworld.com/KINK_DISTORTION.mp3 <- does not show title in metadata
// http://22723.live.streamtheworld.com/KINK_DISTORTION_SC <- does show full metadata!

// https://github.com/andyvans/ultimate-radio/blob/main/src/AudioOut.cpp
//https://github.com/clogboy/EspWebradioPlayer/blob/main/AudioModule.cpp
//https://github.com/PeterDHabermehl/ESP32_bt_mono_speaker/blob/main/AudioTools_MonoSpeaker_rev2_fork.ino

ICYStream streamUrl(DEFAULT_BUFFER_SIZE); 
//StreamCopy copier(vs1053, streamUrl, 1024); // copy url to decoder

// Circular buffer for the radio stream. Will be resized later!
cbuf_ps circBuffer(1024); 

String gUrl;

bool docopy = false;

void webradio_metadata_cb(MetaDataType type, const char* str, int len);
void webradio_handle();
bool webradio_connect(int station_idx);

void webradio_url_set(String urlNew)
{
    Serial.println("Setting URL to " + urlNew);
    gUrl = urlNew;
}

void webradio_init()
{
    // Resize circular buffer
    circBuffer.resize(CONF_WEBRADIO_BUFFERSIZE);

    gUrl = "http://samcloud.spacial.com/api/listen?sid=93462&m=sc&rid=168006";

    // Setup callback
    streamUrl.setMetadataCallback(webradio_metadata_cb);    
}

void webradio_handle()
{
    if(information.audioPlayer.soundMode != WEBRADIO)
        return;
    
    // We implement our own copying function to have more control over the buffering
    uint8_t buffin[CONF_WEBRADIO_BYTESTOGET];
    uint8_t buffout[1024];
    size_t read = 0;    

    if(circBuffer.room() > CONF_WEBRADIO_BYTESTOGET)
    {
      read = streamUrl.readBytes(buffin, min(CONF_WEBRADIO_BYTESTOGET, streamUrl.available()));
      if(read > 0)
      {
        circBuffer.write(buffin, read);

        if((circBuffer.available() > CONF_WEBRADIO_MIN_BYTES) && !docopy)
        {
          LOG_DEBUG("Buffer pre-fill complete");
          docopy = true;
        }
      }
    }   
    else LOG_WARNING("No more room in buffer: %d",  circBuffer.available());

    // Copy from circular buffer to output, when the buffer is full enough
    if(docopy)
    {        
        size_t readout = circBuffer.read(buffout, 100);
        vs1053.write(buffout, readout);
    }

    // If buffer runs empty, disable docopy temporarily to refill the circ buffer
    if(docopy && circBuffer.available() < CONF_WEBRADIO_MIN_BYTES_HALT)
    {
      LOG_ERROR("Buffer underrun!");
      
      information.webRadio.cntUnderruns++;
      docopy = false;
     // webradio_connect(0);
    }

    information.webRadio.bytesAvailable = circBuffer.available();
}

// Connect to a radio stream
bool webradio_connect(int station_idx)
{
    LOG_INFO("CONNECT: %s" , stations[station_idx].name);

    circBuffer.flush();
   
    if(streamUrl.begin(stations[station_idx].url.c_str(),"audio/mp3"))
    {
        LOG_INFO("OK");        
        
        return true;      
    }

    LOG_ERROR("FAIL");
    return false;
}

// Stop the current radio stream
void webradio_disconnect()
{
    LOG_INFO("Disconnect the stream");
    if(information.audioPlayer.soundMode == WEBRADIO)
    {
        LOG_DEBUG("Flushing buffers...");
            
        docopy = false;  
        streamUrl.end();  
        circBuffer.flush();
        vs1053.flush();

        LOG_DEBUG("Done");        
    }
}

// Callback for Icecast metadata
void webradio_metadata_cb(MetaDataType type, const char* str, int len)
{
  LOG_INFO("Metadata received: %s: %s", toStr(type), str);
  //Serial.print(toStr(type));
  //Serial.print(": ");
  //Serial.println(str);

  information.webRadio.title = String(str);
}