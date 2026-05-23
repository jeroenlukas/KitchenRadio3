#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

#include "../system/Filemanager.h"
#include "../system/Logger.h"
#include "../../version.h"
#include "../system/Settings.h"

void websocket_onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

AsyncWebServer webserver(80);

// Create a WebSocket object
AsyncWebSocket websocket("/ws");

String html_header;
String html_footer;
String html_index;

void webserver_begin()
{
    LOGG_INFO("Init webserver");

    // Load header and footer into memory
    html_header =  filemgr_readfile("/www/_header.html");
    html_footer = filemgr_readfile("/www/_footer.html");
    html_index = filemgr_readfile("/www/index.html");
    
    html_header.replace("$DEVICENAME", settings.deviceName);
    html_footer.replace("$VERSION", KR_VERSION);

    webserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request) 
    {
        request->send(200, "text/html", html_header +  information.webRadio.metadataName + " | " + information.webRadio.metadataTitle + html_index +  html_footer);
    });

    // Serve other stuff like css sheets
    webserver.serveStatic("/", LittleFS, "/www/");

    // Websocket
    websocket.onEvent(websocket_onEvent);
    webserver.addHandler(&websocket);

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

void webserver_cleanup(void)
{
    websocket.cleanupClients();
}