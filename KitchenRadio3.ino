#include <LittleFS.h>

#include <Arduino.h>
#include <WiFi.h>

#include <AudioLogger.h>

#include "src/configuration/Config.h"
#include "src/settings/Settings.h"
#include "src/information/Information.h"
#include "version.h"

#include "src/hmi/Cli.h"
#include "src/hmi/Display.h"
#include "src/webserver/Webserver.h"

#include "src/audio/Webradio.h"
#include "src/audio/Audioplayer.h"
#include "src/audio/I2SReceiver.h"

void setup() {
  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);

  delay(100);

  Serial.println("=== KitchenRadio 3! ===");
  Serial.println("Version: " + String(KR_VERSION));
  delay(100);

  // Filesystem
  Serial.println("Start littlefs");
  Serial.println(LittleFS.begin());

  // Settings
  settings_load();

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

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println("Connected");

  // Webserver
  webserver_begin();

  // Audio
  audioplayer_init();

  // Webradio
  webradio_init();

  // I2S
  i2sreceiver_init();

  Serial.println("Init done!");
}

int timer = 0;

void loop() {

  cli_handle();

  webradio_handle();

  i2sreceiver_handle();

  if ((millis() - timer) > 2000) {
    Serial.println("A");
    timer = millis();

    display_draw();
  }
}
