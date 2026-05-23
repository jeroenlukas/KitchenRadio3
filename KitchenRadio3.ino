#include <LittleFS.h>

#include <Arduino.h>
#include <WiFi.h>


#include "src/configuration/Config.h"
#include "src/system/Settings.h"
#include "src/system/Stations.h"
#include "src/system/Logger.h"
#include "src/system/Filemanager.h"
#include "src/information/Information.h"
#include "src/information/Time.h"
#include "src/information/Weather.h"
#include "src/events/Flags.h"
#include "src/events/Events.h"
#include "version.h"

#include "src/hmi/Cli.h"
#include "src/hmi/Display.h"
#include "src/hmi/Frontpanel.h"
#include "src/hmi/Menu.h"
#include "src/hmi/Lamp.h"
#include "src/webserver/Webserver.h"
#include "src/system/Logger.h"
#include "src/system/Profiler.h"

#include "src/audio/Webradio.h"
#include "src/audio/Audioplayer.h"
#include "src/audio/I2SReceiver.h"
#include "src/system/Tickers.h"

TaskHandle_t taskFrontpanel = NULL;


TimeProfile tpEvents("Events");
TimeProfile tpAudio("Audio");

void taskFrontpanel_loop(void* parameter)
{
  for (;;)
  {
    frontpanel_handle();
    
    vTaskDelay(pdMS_TO_TICKS(3));
  }
}

void setup() 
{
  Serial.begin(115200);

  // Logger
  logger_begin();

  delay(100);

    // Display
  display_begin();
  

  LOGG_INFO("=== KitchenRadio 3! ===");
  log_boot("Version: " + String(KR_VERSION));
  log_boot("Compilation: " + String(__DATE__) + " " + String(__TIME__) );
  delay(100);

  // File manager
  log_boot("Start filesystem");
  filemgr_begin();

  // Settings
  log_boot("Load settings");
  settings_load();
  stations_load();
  log_boot("Device name: " + settings.deviceName);

  // CLI
  cli_begin();

  // Frontpanel
  frontpanel_begin();

  // WiFi
  log_boot("Connect to WiFi ...");
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);
  WiFi.setSortMethod(WIFI_CONNECT_AP_BY_SIGNAL);
  WiFi.setHostname("KitchenRadio3");
  WiFi.disconnect();
  WiFi.begin(CONFIG_SECRETS_WIFI_SSID, CONFIG_SECRETS_WIFI_PASSWORD);

  // Audio - we setup the audioplayer while the WiFi is connecting, this shaves a few seconds off the startup time.
  log_boot("Init audioplayer");
  audioplayer_init();

  // Some seconds later
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print('.');
    delay(500);
  }
  
  information.system.ipAddress = WiFi.localIP().toString(); 
  log_boot("WiFi connected: " + information.system.ipAddress);

  // Webserver
  log_boot("Start webserver");
  webserver_begin();

  // Lamp
  log_boot("Init lamp");
  lamp_init();

  // Time
  log_boot("Init time");
  time_begin();
  time_waitForSync();

  // Webradio
  log_boot("Init webradio");
  webradio_init();

  // I2S
  log_boot("Init bluetooth module");
  i2sreceiver_init();

  // Menu system
  menu_begin();

  // Start task for front panel
  xTaskCreatePinnedToCore(
      taskFrontpanel_loop,
      "Frontpanel",
      10000, //4096, // Stack size
      NULL,
      1,
      &taskFrontpanel,
      1); // core0 = wifi/system, core1 = arduino //  Run on Core 0 (shared with WiFi & system tasks)  

  // Tickers
  log_boot("Start tickers");
  tickers_init();

  // Profiler
  profiler.add(&tpEvents);
  profiler.add(&tpAudio);

  // Get weather info
  log_boot("Retrieve weather");
  weather_retrieve();

  // Turn off leds
  frontpanel_leds_handle();

  information.system.bootTimeSeconds = millis() / 1000;
  log_boot("Init done! Boot took " + String(information.system.bootTimeSeconds) + " s" );
  
  delay(2000);
}

// Note:
// Time-sensitive tasks should be run in RTOS tasks since the audio copy functions take quite some time.
// All others are run in the main loop.

void loop() 
{

  // Receive command line commands
  cli_handle();

  // Handle events
  tpEvents.start();
  events_handle();
  tpEvents.stop();

  // Handle webserver
  webserver_cleanup();

  // Handle audio stuff
  tpAudio.start();
  audioplayer_handle();
  tpAudio.stop();

  // Handle buttons etc
  //frontpanel_handle();  
  
  // Update tickers
  tickers_handle();

  // Receive AT commands from Bluetooth slave
  i2sreceiver_serial_handle();

}
