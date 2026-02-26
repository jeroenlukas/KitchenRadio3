#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "configuration/krConfig.h"
#include "information/krInformation.h"

AsyncWebServer webserver(80);


void webserver_begin()
{
    

    webserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request) 
    {
        Serial.println("webserver!");
        request->send(200, "text/html", "<h3>Kitchenradio 3 </h3>" + information.webRadio.title);
    });

    Serial.println("Starting webserver.");
    webserver.begin();
}