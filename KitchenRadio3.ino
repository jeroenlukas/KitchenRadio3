#include <LittleFS.h>

#include <Arduino.h>
#include <WiFi.h>

#include <AudioLogger.h>
#include <AudioTools/Concurrency/RTOS.h>


#include "src/configuration/Config.h"
#include "src/settings/Settings.h"
#include "src/information/Information.h"
#include "src/events/Flags.h"
#include "src/events/Events.h"
#include "version.h"

#include "src/hmi/Cli.h"
#include "src/hmi/Display.h"
#include "src/hmi/Frontpanel.h"
#include "src/webserver/Webserver.h"

#include "src/audio/Webradio.h"
#include "src/audio/Audioplayer.h"
#include "src/audio/I2SReceiver.h"

TaskHandle_t taskFrontpanel = NULL;

void taskFrontpanel_loop(void* parameter)
{
  for (;;)
  {
    frontpanel_handle();
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

void setup() 
{
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

  // Frontpanel
  frontpanel_begin();

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

  // Start task for front panel
  xTaskCreatePinnedToCore(
      taskFrontpanel_loop,
      "Frontpanel",
      4096, // Stack size
      NULL,
      1,
      &taskFrontpanel,
      0); // Run on Core 0 (shared with WiFi & system tasks)  
}


int timer = 0;

// Note:
// Time-sensitive tasks should be run in RTOS tasks since the audio copy functions take quite some time.
// All others are run in the main loop.

void loop() 
{

  // Receive command line commands
  cli_handle();

  // Handle events
  events_handle();

  // Handle audio stuff
  audioplayer_handle();

  // Handle buttons etc
  // frontpanel_handle();

  // Receive AT commands from Bluetooth slave
  i2sreceiver_serial_handle();



  if ((millis() - timer) > 1000) 
  {
    Serial.println("A");
    timer = millis();

    display_draw();
  }

  
}
