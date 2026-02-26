#include <Arduino.h>
#include <WiFi.h>

#include <AudioLogger.h>

#include "configuration/krConfig.h"
#include "information/krInformation.h"

#include "hmi/krCli.h"
#include "hmi/krDisplay.h"
#include "webserver/krWebserver.h"
#include "audioplayer/krWebradio.h"

#include "audioplayer/krAudioplayer.h"

/*
TaskHandle_t copyTask = NULL;

void ProcessDevices(void* parameter)
{
    Serial.println("!COPY");
    audioplayer_copy();
}
*/

void setup() 
{
  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);

  delay(100);

  Serial.print("KitchenRadio 3!");
  delay(100);  

  // Display
  Serial.println("Init display");
  display_begin();
  display_draw_startup();

  // CLI
  cli_begin();

  // WiFi
  Serial.println("Connect to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);
  WiFi.setSortMethod(WIFI_CONNECT_AP_BY_SIGNAL);
  WiFi.setHostname("KitchenRadio3");
  WiFi.disconnect();

  WiFi.begin(CONFIG_SECRETS_WIFI_SSID, CONFIG_SECRETS_WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED)
  {
      Serial.print('.');
      delay(500);
  }
  Serial.println("Connected");

  // Webserver
  Serial.println("Init webserver");
  webserver_begin();

  // Webradio
  webradio_begin();
  //audioplayer_begin();
  

  // tasik
 /* xTaskCreatePinnedToCore(
    ProcessDevices,
    "Device",
    1024*100, // Reduced for just wled and device controls
    NULL,
    1,
    &copyTask,
    0); // Core 0 (shared with WiFi & system tasks)  */

    
}

int timer =0;

void loop() 
{

  cli_handle();

  
  webradio_handle();
  //audioplayer_copy();

  if((millis() - timer) > 1000)
  {
    Serial.println("A" );
    timer = millis();

    display_draw();

  }
}
