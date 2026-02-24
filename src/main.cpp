#include <Arduino.h>
#include <WiFi.h>

#include "config/krConfig.h"

#include "webserver/krWebserver.h"
#include "hmi/krDisplay.h"

void setup() 
{
  Serial.begin(115200);

  delay(100);

  Serial.print("KitchenRadio 3!");
  delay(100);  

  Serial.println("Init display");
  display_begin();


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

  Serial.println("Init webserver");
  webserver_begin();

  

}

int timer =0;

void loop() 
{
  if((millis() - timer) > 1000)
  {
    Serial.println("A" );
    timer = millis();

    display_draw();

  }
}
