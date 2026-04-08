#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "../system/Logger.h"

AsyncWebServer webserver(80);


void webserver_begin()
{
    LOGG_INFO("Init webserver");

    webserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request) 
    {
        LOGG_DEBUG("webserver!");
        request->send(200, "text/html", "<h3>Kitchenradio 3 </h3>" + information.webRadio.title);
    });

    LOGG_INFO("Starting webserver.");
    webserver.begin();
}