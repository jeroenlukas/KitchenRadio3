#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <ElegantOTA.h>

#include "../system/Filemanager.h"
#include "../system/Logger.h"
#include "../../version.h"
#include "../system/Settings.h"
#include "../audio/Audioplayer.h"
#include "../system/Stations.h"
#include "../hmi/Display.h"
#include "../hmi/Buzzer.h"

void websocket_onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void webserver_ota_start();
void webserver_ota_end(bool success);
void webserver_ota_progress(size_t current, size_t final);

AsyncWebServer webserver(80);

// Create a WebSocket object
AsyncWebSocket websocket("/ws");

String html_header;
String html_footer;
String html_index;
String html_config;
String html_stations;

void webserver_begin()
{
  LOGG_INFO("Init webserver");

  // Load header and footer into memory
  html_header =  filemgr_readfile("/www/_header.html");
  html_footer = filemgr_readfile("/www/_footer.html");
  html_index = filemgr_readfile("/www/index.html");
  html_config = filemgr_readfile("/www/config.html");
  html_stations = filemgr_readfile("/www/stations.html");
  
  html_header.replace("$DEVICENAME", settings.deviceName);
  html_footer.replace("$VERSION", KR_VERSION);

  // Index
  webserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request) 
  {
      request->send(200, "text/html", html_header +  information.webRadio.metadataName + " | " + information.webRadio.metadataTitle + html_index +  html_footer);
  });

  // Config GET
  webserver.on("/config", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(200, "text/html", html_header + html_config +  html_footer);
  });

  // Config POST
  webserver.on("/config", HTTP_POST, [](AsyncWebServerRequest *request) {
    // display params
    size_t count = request->params();
    for (size_t i = 0; i < count; i++) {
      const AsyncWebParameter *p = request->getParam(i);
      Serial.printf("PARAM[%u]: %s = %s\n", i, p->name().c_str(), p->value().c_str());
    }

    // get who param
    String new_content;
    if (request->hasParam("config_content", true)) {
      new_content = request->getParam("config_content", true)->value();

      // Write to settings.json
      filemgr_writefile("/settings/config.yaml", new_content);
      
      // Reload config
      display_popup("Settings stored");
      delayMicroseconds(500000);      
      settings_load();

      // Return to config file
      request->send(200, "text/html", html_header + html_config +  html_footer);
    }
    else if (request->hasParam("secrets_content", true)) {
      new_content = request->getParam("secrets_content", true)->value();

      // Write to settings.json
      filemgr_writefile("/settings/secrets.yaml", new_content);
      
      // Reload config
      display_popup("Secrets stored");
      delayMicroseconds(500000);      
      settings_load();

      // Return to config file
      request->send(200, "text/html", html_header + html_config +  html_footer);
    }
    
  });

  // Stations GET
  webserver.on("/stations", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(200, "text/html", html_header + html_stations +  html_footer);
  });

  // Stations POST
  webserver.on("/stations", HTTP_POST, [](AsyncWebServerRequest *request) {
    // display params
    size_t count = request->params();
    for (size_t i = 0; i < count; i++) {
      const AsyncWebParameter *p = request->getParam(i);
      Serial.printf("PARAM[%u]: %s = %s\n", i, p->name().c_str(), p->value().c_str());
    }

    // get who param
    String new_content;
    if (request->hasParam("stations_content", true)) {
      new_content = request->getParam("stations_content", true)->value();

      // Write to settings.json
      filemgr_writefile("/settings/stations.yaml", new_content);
      
      // Reload config
      display_popup("Stations stored");
      delayMicroseconds(500000);      
      stations_load();

      // Return to config file
      request->send(200, "text/html", html_header + html_stations +  html_footer);
    }
    
  });



  // Serve other stuff like css sheets
  webserver.serveStatic("/", LittleFS, "/www/");

  // Websocket
  websocket.onEvent(websocket_onEvent);
  webserver.addHandler(&websocket);

  // ElegantOTA  
  ElegantOTA.begin(&webserver);
  ElegantOTA.onStart(webserver_ota_start);
  ElegantOTA.onEnd(webserver_ota_end);
  ElegantOTA.onProgress(webserver_ota_progress);

  // Start webserver
  webserver.begin();
}

void websocket_handlemessage(void *arg, uint8_t *data, size_t len) 
{
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
  {      
    data[len] = '\0';

    String message = String((char*)data);
    LOGG_DEBUG("Received:" + message);

    JsonDocument docMessage;
    if(deserializeJson(docMessage, message)!= DeserializationError::Ok)
    {
        LOGG_ERROR("Error: deser error!");
        return;
    }
    LOGG_INFO("Deserialization ok");

    // If its a get command
    if(docMessage["get"])
    {
      LOGG_DEBUG("Get: " + String(docMessage["get"]));
      if(docMessage["get"] == "configuration")
      {
        String configuration = filemgr_readfile("/settings/config.yaml");

        JsonDocument docReply;
        String jsonString;
        docReply["config_content"] = configuration;

        serializeJson(docReply, jsonString);
        websocket.textAll(jsonString);        
      }
      else if(docMessage["get"] == "stations")
      {
        String stations = filemgr_readfile("/settings/stations.yaml");

        JsonDocument docReply;
        String jsonString;
        docReply["stations_content"] = stations;

        serializeJson(docReply, jsonString);
        websocket.textAll(jsonString);
      }
      else if(docMessage["get"] == "secrets")
      {
        String configuration = filemgr_readfile("/settings/secrets.yaml");

        JsonDocument docReply;
        String jsonString;
        docReply["secrets_content"] = configuration;

        serializeJson(docReply, jsonString);
        websocket.textAll(jsonString);        
      }
    }

    else if(docMessage["soundmode"])
    {
      if(docMessage["soundmode"] == "off")
        audioplayer_mode_set(OFF);

      else if(docMessage["soundmode"] == "radio")
        audioplayer_mode_set(WEBRADIO);

      else if(docMessage["soundmode"] == "bluetooth")
        audioplayer_mode_set(BLUETOOTH);

    }

    else LOGG_WARNING("Unknown json command");
    

  }
}

void websocket_onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) 
  {
    case WS_EVT_CONNECT:
      LOGG_INFO("WebSocket client #" + String(client->id()) + " connected from " + client->remoteIP().toString());
      break;
    case WS_EVT_DISCONNECT:
      LOGG_INFO("WebSocket client #" + String(client->id()) + " disconnected" );
      break;
    case WS_EVT_DATA:
      websocket_handlemessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void webserver_handle(void)
{
    websocket.cleanupClients();
    ElegantOTA.loop();
}

void webserver_ota_start()
{
  buzzer_beep(500 * 1000);
  display_popup("OTA start");  
  LOGG_INFO("OTA start");
}

void webserver_ota_end(bool success)
{
  buzzer_beep(250 * 1000);
  
  if(success)
  {
    display_popup("OTA succeeded. I will reboot!");
    LOGG_INFO("OTA succeeded, I will reboot!");
  }
  else
  {
    display_popup("OTA failed!");
    LOGG_ERROR("OTA failed!");
  }
}

void webserver_ota_progress(size_t current, size_t final)
{
  static uint32_t ota_progress_millis = 0;

  if (millis() - ota_progress_millis > 500) 
  {
    ota_progress_millis = millis();
    double perc = ((double)current / (double)final) * 100.0;
    String message = String("OTA progress: " + String(perc, 0) + "%");
    LOGG_INFO(message);
    display_popup(message);
  }
}