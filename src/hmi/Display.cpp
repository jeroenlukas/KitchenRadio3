#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>

#include "u8g2_font_climacons_40.h"
#include "../system/Logger.h"
#include "../information/Time.h"
#include "../system/Settings.h"
#include "../system/Profiler.h"
#include "../information/Weather.h"
#include "XbmIcons.h"
#include "Menu.h"

TimeProfile tpDisplay("Display");

U8G2_SSD1322_NHD_256X64_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ CONFIG_PIN_HSPI_CS, /* dc=*/ CONFIG_PIN_HSPI_DC, /* reset=*/ 9);	// Enable U8G2_16BIT in u8g2.h

SPIClass *hspi = NULL;

int16_t display_audio_title_scroll_offset = 0;
bool display_audio_title_scroll_dir = true;
uint16_t display_audio_title_width = 0;

void display_begin()
{
  LOGG_INFO("Display init");
  hspi = new SPIClass(HSPI);
  hspi->begin(CONFIG_PIN_HSPI_SCK, CONFIG_PIN_HSPI_MISO, CONFIG_PIN_HSPI_MOSI, CONFIG_PIN_HSPI_CS);  
  u8g2.begin();

  profiler.add(&tpDisplay);
}

// === Home screen ===
void display_draw_home()
{
  //u8g2.setFont(u8g2_font_likeminecraft_te);
  
  // Clock
  u8g2.setFont(FONT_CLOCK);
  u8g2.setCursor(POSX_CLOCK, POSY_CLOCK);
  u8g2.print(u8x8_u8toa(information.hour, 2)); 
  u8g2.drawStr(POSX_CLOCK + 30, POSY_CLOCK - 2, ":");
  u8g2.setCursor(POSX_CLOCK + 39, POSY_CLOCK);
  u8g2.print(u8x8_u8toa(information.minute, 2));
  
  // Date
  u8g2.setFont(FONT_S);
  u8g2.drawStr(POSX_CLOCK + 10, POSY_CLOCK + 12, (information.dateMid).c_str());

  // Weather
    u8g2.setFont(FONT_WEATHERICONS);
    int weatherglyph = 0;
    // https://openweathermap.org/weather-conditions

    u8g2.drawGlyph(3, 35, weather_icon_to_glyph(information.weather.icon));
    u8g2.setFont(u8g2_font_lastapprenticebold_te);
    uint8_t w = u8g2.drawStr(42, 18,(String(information.weather.temperature,1) + "  C").c_str());
    u8g2.drawGlyph((42 + w) - 13, 18, 0x00b0);
    u8g2.setFont(FONT_M);
    u8g2.drawStr(42, 28,(String(information.weather.windSpeedBft) + " Bft").c_str());
    u8g2.drawStr(42, 38,(String(information.weather.stateShort)).c_str());
    u8g2.setFont(FONT_S);
  
  switch(information.audioPlayer.soundMode)
  {
    case OFF:
      // ...
      break;
    case WEBRADIO:
      //u8g2.drawStr(10, 36, String("Radio: " + information.webRadio.metadataName + " | " + information.webRadio.metadataTitle).c_str());
      u8g2.drawXBM(POSX_AUDIO_ICON, POSY_AUDIO_ICON-15, xbm_radio_width, xbm_radio_height, xbm_radio_bits);
      
      // Draw buffer fill percentage, station index + count
      u8g2.setFont(FONT_S);
      u8g2.drawStr(POSX_AUDIO -20, POSY_AUDIO-7, (String(information.webRadio.station_index + 1) + "/" + String(information.webRadio.station_count)).c_str() );
      u8g2.drawStr(POSX_AUDIO -20, POSY_AUDIO+1, (String(information.webRadio.bufferPercentage) + "%").c_str() );

      // Draw station name in clipwindow
      u8g2.setFont(FONT_AUDIO);
      u8g2.setClipWindow(POSX_AUDIO, 43, 224, 64);
      display_audio_title_width = u8g2.drawStr(POSX_AUDIO + display_audio_title_scroll_offset, POSY_AUDIO, String(information.webRadio.metadataName + " | " + information.webRadio.metadataTitle).c_str());
      u8g2.setMaxClipWindow();
      break;
    case BLUETOOTH:
      //u8g2.drawStr(10, 36, String("Bt: " + information.audioPlayer.bluetoothTitle).c_str());
      u8g2.drawXBM(POSX_AUDIO_ICON, POSY_AUDIO_ICON-16, xbm_bluetooth_width, xbm_bluetooth_height, xbm_bluetooth_bits);

      // Draw bluetooth title in clipwindow
      u8g2.setFont(FONT_AUDIO);
      u8g2.setClipWindow(POSX_AUDIO, 43, 224, 64);              
      if(information.audioPlayer.bluetoothArtist != "")
        display_audio_title_width = u8g2.drawStr(POSX_AUDIO + display_audio_title_scroll_offset, POSY_AUDIO, String(information.audioPlayer.bluetoothArtist + " - " + information.audioPlayer.bluetoothTitle).c_str());
      else
        display_audio_title_width = u8g2.drawStr(POSX_AUDIO + display_audio_title_scroll_offset, POSY_AUDIO, String(information.audioPlayer.bluetoothConnectionStateStr).c_str());
      u8g2.setMaxClipWindow();
      
      // Draw audio state icon
      u8g2.setFont(u8g2_font_twelvedings_t_all);
      switch(information.audioPlayer.bluetoothMode)
      {
        case BT_OFF:                  
          u8g2.drawGlyph(POSX_AUDIO - 20, POSY_AUDIO, 0);
          break;
        case BT_NOTCONNECTED:
        case BT_CONNECTED:
        case BT_CONNECTING:
        case BT_DISCONNECTING:
          u8g2.drawGlyph(POSX_AUDIO - 20, POSY_AUDIO, 63);
          break;
        case BT_PAUSED:
          u8g2.drawGlyph(POSX_AUDIO - 20, POSY_AUDIO, 69);
          break;
        case BT_PLAYING:
          u8g2.drawGlyph(POSX_AUDIO - 20, POSY_AUDIO, 68);
          break;
        case BT_UNKNOWN:            
          u8g2.drawStr(POSX_AUDIO - 20, POSY_AUDIO, "?");
          break;
      }
      break;
      break;
    default:
      break;

  }
  
  
  u8g2.drawLine(0, 44, 256, 44);      

  // Volume indicator
  u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
  if(information.audioPlayer.mute)
    u8g2.drawGlyph(228, POSY_AUDIO, 279);
  else
    u8g2.drawGlyph(228, POSY_AUDIO, 277);
  u8g2.setFont(FONT_S);
  u8g2.drawStr(238, POSY_AUDIO-1, (String(information.audioPlayer.volume) + "%").c_str());
}

// === Menu screen ===
void display_draw_menu()
{
  u8g2.setFont(u8g2_font_likeminecraft_te);  
  
  MenuItem* item = menuMgr.currentMenu()->getSelectedItem();

  switch(item->getType())
  {
    case VALUE_ITEM:  
      {    
        ValueItem* val = (ValueItem*)item;  
        u8g2.drawStr(10, 15, menuMgr.currentMenu()->getName()); // Draw menu name
        u8g2.drawStr(10, 36, item->getName()); // Draw item name
        u8g2.drawStr(70, 36, String(val->getValue()).c_str());  // Draw item value
      }
      break;

    case CUSTOMINFO_ITEM:
      {
        InfoItem* ii = (InfoItem*)item;
        u8g2.drawStr(80, POSY_AUDIO, item->getName()); // Draw name
        ii->show();
      }
      break;

  }

  // Always draw the footer
  u8g2.drawLine(0, 48, 256, 48);

  u8g2.setFont(u8g2_font_6x12_m_symbols);
  u8g2.drawGlyph(10, 62, 8626); // Back icon

  u8g2.setFont(FONT_S);
  u8g2.drawStr(238, POSY_AUDIO-1, String(String(menuMgr.currentMenu()->getItemIndex()+1) + "/"  + String(menuMgr.currentMenu()->getItemCount()) ).c_str()); // Index
 
}

void display_draw()
{
  tpDisplay.start();

  u8g2.firstPage();
  do 
  {      
    if(menuMgr.isActive())
    {
      display_draw_menu();
    }
    else display_draw_home();
    
  } while ( u8g2.nextPage() );

  tpDisplay.stop();
}

void display_draw_startup()
{
    u8g2.firstPage();
    do 
    {      
      u8g2.setFont(u8g2_font_minicute_tr);
      
      u8g2.drawStr(10, 26, "=== KitchenRadio 3 ===");      
      
    } while ( u8g2.nextPage() );
}


void display_set_brightness(uint8_t brightness)
{
  // Contrast (0-100)
  uint8_t contrast = map(brightness, 0, 100, 0, 50);
  u8g2.setContrast(contrast);

  // Pre charge voltage (0-31) 0 is 0ff
  uint8_t pcv = map(brightness, 0, 100, 0, 31);

  u8g2.sendF("ca", 0xBB, pcv);  
}

void display_set_brightness_auto()
{
  //uint8_t brightness = map(information.system.ldr, 0, 100, CONF_DISPLAY_AUTO_BRIGHTNESS_MIN, CONF_DISPLAY_AUTO_BRIGHTNESS_MAX);
  
  int br_max = int(CONF_DISPLAY_AUTO_BRIGHTNESS_MAX);
  int br_min = int(CONF_DISPLAY_AUTO_BRIGHTNESS_MIN);

  if(br_max == 0) br_max = 100;

  uint8_t brightness = map(information.system.ldr, 0, 100, br_min, br_max);
  
  display_set_brightness(brightness);  
}

// ===  Custom info items ===

// System stats
void display_draw_custominfo_system()
{
  u8g2.setFont(FONT_S);
  u8g2.drawStr(10, 12, String(settings.deviceName).c_str());
  u8g2.drawStr(10, 22, "IP: ");         u8g2.drawStr(70, 22, information.system.ipAddress.c_str());
  u8g2.drawStr(10, 32, "WiFi RSSI:");        u8g2.drawStr(70, 32, (String(information.system.wifiRSSI) + " dBm").c_str());      
    
  if(information.audioPlayer.soundMode == BLUETOOTH){
    u8g2.drawStr(10, 42, "BT RSSI:");   
    u8g2.drawStr(70, 42, (String(information.audioPlayer.bluetoothRSSI) + " dBm").c_str()); // Show bluetooth RSSI
  }
  else if(information.audioPlayer.soundMode == WEBRADIO){
    u8g2.drawStr(10, 42, "Buffer:"); 
    u8g2.drawStr(70, 42, (String(information.webRadio.bytesAvailable / 1024) + " kB").c_str());   
  }

  u8g2.drawStr(150, 12, "Uptime:");     u8g2.drawStr(200, 12, time_convert(information.system.uptimeSeconds).c_str());
  u8g2.drawStr(150, 22, "Amb.light:");  u8g2.drawStr(200, 22, (String(information.system.ldr) + "%").c_str());
  u8g2.drawStr(150, 32, "Rst reason:");  u8g2.drawStr(200, 32, (String(information.system.lastResetReason)).c_str());
  u8g2.drawStr(150, 42, "Underruns:");  u8g2.drawStr(200, 42, (String(information.webRadio.cntUnderruns)).c_str());
}

// Smiley icons
void display_draw_custominfo_smiley()
{
  uint16_t rand = map(information.minute, 0, 59, 48, 688); //random(48, 688);
  u8g2.setFont(u8g2_font_streamline_all_t);
  u8g2.drawGlyph(80,30, rand);
  u8g2.drawGlyph(150,47,rand+1);
}

// Weather info
void display_draw_custominfo_weather()
{
  u8g2.setFont(FONT_S);
  u8g2.drawStr(10, 12, "Wind:");       u8g2.drawStr(70, 12, (String(information.weather.windSpeedKmh) + "km/h").c_str());
  u8g2.drawStr(10, 22, "Temp:");       u8g2.drawStr(70, 22, (String(information.weather.temperature) + " 'C").c_str());
  u8g2.drawStr(10, 32, "Feels like:"); u8g2.drawStr(70, 32, (String(information.weather.temperature_feelslike) + " 'C").c_str());
  u8g2.drawStr(10, 42, "Humidity:");   u8g2.drawStr(70, 42, (String(information.weather.humidity) + "%").c_str());

  u8g2.drawStr(150, 12, "Pressure:");  u8g2.drawStr(200, 12, (String(information.weather.pressure) + " hPa").c_str());
  u8g2.drawStr(150, 22, "Sunrise:");   u8g2.drawStr(200, 22, (String(information.weather.sunrise_str)).c_str());
  u8g2.drawStr(150, 32, "Sunset:");    u8g2.drawStr(200, 32, (String(information.weather.sunset_str)).c_str());
}


void display_reset_scroll()
{
  display_audio_title_scroll_offset= 0;
  display_audio_title_scroll_dir=0;
}

void display_update_scroll_offset()
{
  #define BOX_WIDTH (224 - POSX_AUDIO)
  if(display_audio_title_width < BOX_WIDTH)
  {
    display_audio_title_scroll_offset = 0;
    return;
  }

  if(display_audio_title_scroll_dir)
  {
    display_audio_title_scroll_offset+=2;
  }
  else
    display_audio_title_scroll_offset-=2;

  // Reverse 
  if(display_audio_title_scroll_offset > 2)
    display_audio_title_scroll_dir = !display_audio_title_scroll_dir;

  else if(display_audio_title_scroll_offset < ((BOX_WIDTH - display_audio_title_width)-3))
    display_audio_title_scroll_dir = !display_audio_title_scroll_dir;
}