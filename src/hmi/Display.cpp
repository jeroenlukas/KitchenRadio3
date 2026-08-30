#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <Ticker.h>

#include "u8g2_font_climacons_40.h"
#include "../system/Logger.h"
#include "../information/Time.h"
#include "../system/Settings.h"
#include "../system/Profiler.h"
#include "../information/Weather.h"
#include "../events/Flags.h"
#include "../system/Stations.h"
#include "../../version.h"
#include "XbmIcons.h"
#include "Menu.h"

//void display_popup(String message);

TimeProfile tpDisplay("Display");

U8G2_SSD1322_NHD_256X64_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/CONFIG_PIN_HSPI_CS, /* dc=*/CONFIG_PIN_HSPI_DC, /* reset=*/9);  // Enable U8G2_16BIT in u8g2.h

SPIClass* hspi = NULL;

int16_t display_audio_title_scroll_offset = 0;
bool display_audio_title_scroll_dir = true;
uint16_t display_audio_title_width = 0;

Ticker ticker_popup;
void ticker_popup_cb();
String popup_message;
bool popup_show = false;

void display_draw_triangle_rotated(int cx, int cy, float angle);

void display_begin() {
  LOGG_INFO("Display init");
  hspi = new SPIClass(HSPI);
  hspi->begin(CONFIG_PIN_HSPI_SCK, CONFIG_PIN_HSPI_MISO, CONFIG_PIN_HSPI_MOSI, CONFIG_PIN_HSPI_CS);
  u8g2.begin();

  profiler.add(&tpDisplay);
}

// === Home screen ===
void display_draw_home() {
  // Clock
  u8g2.setFont(FONT_CLOCK);
  u8g2.setCursor(POSX_CLOCK, POSY_CLOCK);
  u8g2.print(u8x8_u8toa(information.clock.hour, 2));
  if(information.clock.colon_state) u8g2.drawStr(POSX_CLOCK + 30, POSY_CLOCK - 2, ":");
  u8g2.setCursor(POSX_CLOCK + 39, POSY_CLOCK);
  u8g2.print(u8x8_u8toa(information.clock.minute, 2));

  // Date
  u8g2.setFont(FONT_S);
  u8g2.drawStr(POSX_CLOCK + 10, POSY_CLOCK + 12, (information.clock.dateMid).c_str());

  // Weather
  u8g2.setFont(FONT_WEATHERICONS);
  int weatherglyph = 0;
  // https://openweathermap.org/weather-conditions

  u8g2.drawGlyph(3, 42, weather_icon_to_glyph(information.weather.icon));
  u8g2.setFont(FONT_WEATHER_TEMPERATURE);
  uint8_t w = u8g2.drawStr(42, 18, (String(information.weather.temperature, 1) + "  C").c_str());
  u8g2.drawGlyph((42 + w) - 13, 18, 0x00b0);
  u8g2.setFont(FONT_M);
  u8g2.drawStr(42, 28, (String(information.weather.windSpeedBft) + " Bft " + information.weather.wind_direction_str).c_str());

 // drawWindArrow(100, 28, 10, information.system.uptimeSeconds);
  //drawRotatedTriangle(100, 28, 10, 10, information.system.uptimeSeconds * 2);
  
  display_draw_triangle_rotated(100, 24, information.weather.wind_direction_deg);

  u8g2.drawStr(42, 38, (String(information.weather.stateShort)).c_str());
  u8g2.setFont(FONT_S);

  // Alarm (if active)
  if (information.alarm.state != ALARM_STATE_OFF) {
    u8g2.setFont(u8g2_font_open_iconic_all_2x_t);
    u8g2.drawGlyph(POSX_ALARM, POSY_ALARM + 2, 93);
    u8g2.setFont(FONT_ALARM);
    u8g2.drawFrame(POSX_ALARM - 5, POSY_ALARM - 18, 60, 25);

    if (information.alarm.state == ALARM_STATE_COUNTDOWN || information.alarm.state == ALARM_STATE_PAUSED) {
      u8g2.drawStr(POSX_ALARM + 20, POSY_ALARM, (information.alarm.countdown_minsec).c_str());
    }

    else if (information.alarm.state == ALARM_STATE_BUZZING) {
      u8g2.drawStr(POSX_ALARM + 20, POSY_ALARM, "ALARM!");
    }
  }

  // Audio
  switch (information.audioPlayer.soundMode) {
    case OFF:
      // ...
      break;
    case WEBRADIO:
      u8g2.drawXBM(POSX_AUDIO_ICON, POSY_AUDIO_ICON - 15, xbm_radio_width, xbm_radio_height, xbm_radio_bits);

      // Draw buffer fill percentage, station index + count
      u8g2.setFont(FONT_S);
      u8g2.drawStr(POSX_AUDIO - 20, POSY_AUDIO - 7, (String(information.webRadio.station_index_select + 1) + "/" + String(information.webRadio.station_count)).c_str());
      u8g2.drawStr(POSX_AUDIO - 20, POSY_AUDIO + 1, (String(information.webRadio.bufferPercentage) + "%").c_str());

      // Draw station name in clipwindow
      u8g2.setFont(FONT_AUDIO);
      if (information.webRadio.station_index == information.webRadio.station_index_select) {
        u8g2.setClipWindow(POSX_AUDIO, 43, 224, 64);
        if (information.webRadio.metadataTitle != "")
          display_audio_title_width = u8g2.drawStr(POSX_AUDIO + display_audio_title_scroll_offset, POSY_AUDIO, String(information.webRadio.metadataName + " | " + information.webRadio.metadataTitle).c_str());
        else
          display_audio_title_width = u8g2.drawStr(POSX_AUDIO + display_audio_title_scroll_offset, POSY_AUDIO, String(information.webRadio.metadataName).c_str());
        u8g2.setMaxClipWindow();
      } else  // Draw select station
      {
        u8g2.drawStr(POSX_AUDIO, POSY_AUDIO, String(">>> " + stations[information.webRadio.station_index_select].name).c_str());
      }
      break;
    case BLUETOOTH:
      //u8g2.drawStr(10, 36, String("Bt: " + information.audioPlayer.bluetoothTitle).c_str());
      u8g2.drawXBM(POSX_AUDIO_ICON, POSY_AUDIO_ICON - 16, xbm_bluetooth_width, xbm_bluetooth_height, xbm_bluetooth_bits);

      // Draw bluetooth title in clipwindow
      u8g2.setFont(FONT_AUDIO);
      u8g2.setClipWindow(POSX_AUDIO, 43, 224, 64);
      if (information.audioPlayer.bluetoothArtist != "")
        display_audio_title_width = u8g2.drawStr(POSX_AUDIO + display_audio_title_scroll_offset, POSY_AUDIO, String(information.audioPlayer.bluetoothArtist + " - " + information.audioPlayer.bluetoothTitle).c_str());
      else
        display_audio_title_width = u8g2.drawStr(POSX_AUDIO + display_audio_title_scroll_offset, POSY_AUDIO, String(information.audioPlayer.bluetoothConnectionStateStr).c_str());
      u8g2.setMaxClipWindow();

      // Draw audio state icon
      u8g2.setFont(u8g2_font_twelvedings_t_all);
      switch (information.audioPlayer.bluetoothMode) {
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
      
    default:
      break;
  }

  u8g2.drawLine(0, 44, 256, 44);

  // Volume indicator
  u8g2.setFont(u8g2_font_open_iconic_all_1x_t);
  if (information.audioPlayer.mute)
    u8g2.drawGlyph(228, POSY_AUDIO, 279);
  else
    u8g2.drawGlyph(228, POSY_AUDIO, 277);
  u8g2.setFont(FONT_S);
  u8g2.drawStr(238, POSY_AUDIO - 1, (String(information.audioPlayer.volume) + "%").c_str());
}

// === Menu screen ===
void display_draw_menu() {
  MenuItem* item = menuMgr.currentMenu()->getSelectedItem();

  // For all setting types, draw the name of the setting.
  // For custominfo types, this is drawn in the path.

  if (item->getType() == CUSTOMINFO_ITEM) 
  {
    // Draw breadcrumb + custominfo title
    u8g2.setFont(FONT_MENUPATH);
    u8g2.drawStr(POSX_MENUPATH, POSY_MENUPATH, String(menuMgr.currentMenu()->getPath() + " > " + item->getName()).c_str());  // Draw menu name/path
  } 
  else if (item->getType() == MENU_ITEM) 
  {
    Menu* mn = (Menu*)item;

    if(! mn->hasCustomDisplay())
    {
      u8g2.setFont(FONT_MENUITEM);
      u8g2.drawStr(POSX_MENUITEM, POSY_MENUITEM, String(String(item->getName()) + "...").c_str());  // Draw item name
      u8g2.setFont(FONT_MENUPATH);
      u8g2.drawStr(POSX_MENUPATH, POSY_MENUPATH, menuMgr.currentMenu()->getPath().c_str());  // Draw menu name/path
    }
    else
    {
      u8g2.setFont(FONT_MENUPATH);
      u8g2.drawStr(POSX_MENUPATH, POSY_MENUPATH, String(menuMgr.currentMenu()->getPath() + " > " + item->getName()).c_str());  // Draw menu name/path
    }

    // Indicate that its a submenu which can be entered
    u8g2.setFont(u8g2_font_open_iconic_all_1x_t);    
    u8g2.drawGlyph(220, POSY_AUDIO, 64); 
  } 
  else 
  {
    if(item->getType() != ACTION_ITEM)
    {
      u8g2.setFont(FONT_MENUITEM);
      u8g2.drawStr(POSX_MENUITEM, POSY_MENUITEM, item->getName());  // Draw item name
    }

    // Draw breadcrumb
    u8g2.setFont(FONT_MENUPATH);
    u8g2.drawStr(POSX_MENUPATH, POSY_MENUPATH, menuMgr.currentMenu()->getPath().c_str());  // Draw menu name/path
  }


  u8g2.setFont(FONT_MENUITEM);

  switch (item->getType()) 
  {
    case INT_ITEM:
      {
        IntItem* val = (IntItem*)item;

        int width_frame = u8g2.getStrWidth(String(val->maxVal).c_str());
        u8g2.drawFrame(POSX_MENUITEM_VALUE - 4, POSY_MENUITEM - 10, width_frame + 8, 14);

        u8g2.drawStr(POSX_MENUITEM_VALUE, POSY_MENUITEM, String(val->getValue()).c_str());  // Draw item value
      }
      break;

    case FLOAT_ITEM:
      {
        FloatItem* val = (FloatItem*)item;

        int width_frame = u8g2.getStrWidth("0") * (val->decimals + 2);
        u8g2.drawFrame(POSX_MENUITEM_VALUE - 4, POSY_MENUITEM - 10, width_frame + 8, 14);

        u8g2.drawStr(POSX_MENUITEM_VALUE, POSY_MENUITEM, String(val->getValue(), val->decimals).c_str());  // Draw item value
      }
      break;

    case MINSEC_ITEM:
      {
        MinSecItem* val = (MinSecItem*)item;

        //int width_frame = u8g2.getStrWidth(String(val->maxVal).c_str());
        //u8g2.drawFrame(POSX_MENUITEM_VALUE - 4, POSY_MENUITEM - 10, width_frame + 8, 14);

        u8g2.drawStr(POSX_MENUITEM_VALUE, POSY_MENUITEM, String(val->getValueMinSec()).c_str());  // Draw item value
      }
      break;

    case OPTION_ITEM:
      {
        OptionItem* oi = (OptionItem*)item;

        int width_frame = 120;  //u8g2.getStrWidth(String(val->maxVal).c_str());
        u8g2.drawFrame(POSX_MENUITEM_VALUE - 4, POSY_MENUITEM - 10, width_frame + 8, 14);

        u8g2.drawStr(POSX_MENUITEM_VALUE, POSY_MENUITEM, String(oi->getValueString()).c_str());  // Draw item value
      }
      break;

    case BOOL_ITEM:
      {
        BoolItem* bi = (BoolItem*)item;

        int width_frame = 120;
        u8g2.drawFrame(POSX_MENUITEM_VALUE - 4, POSY_MENUITEM - 10, width_frame + 8, 14);

        u8g2.drawStr(POSX_MENUITEM_VALUE, POSY_MENUITEM, String(bi->getValueString()).c_str());  // Draw item value
      }
      break;

    case MENU_ITEM:
      {
        Menu* mn = (Menu*)item;
        if(mn->hasCustomDisplay())
          mn->show();

        else
          u8g2.drawStr(POSX_MENUITEM_VALUE, POSY_MENUITEM, "(press TUNE to enter)");  // Draw item value
      }
      break;

    case CUSTOMINFO_ITEM:
      {
        InfoItem* ii = (InfoItem*)item;
        ii->show();
      }
      break;

    case ACTION_ITEM:
      {
        ActionItem* ai = (ActionItem*)item;
        
        int len = u8g2.getStrWidth(ai->getName());

        u8g2.drawRFrame(POSX_CENTER - (len / 2), POSY_CENTER - 22 , len + 14, 20, 4);
        u8g2.drawStr(POSX_CENTER - (len / 2) + 7, POSY_CENTER - 10,  ai->getName());
      }
      break;

    default:
      {
        LOGG_ERROR("Unknown menuitem type!");
      }
      break;
  }



  // Always draw the footer
  u8g2.drawLine(0, 48, 256, 48);

  u8g2.setFont(u8g2_font_6x12_m_symbols);
  u8g2.drawGlyph(10, 62, 8626);  // Back icon

  u8g2.setFont(FONT_S);
  u8g2.drawStr(238, POSY_AUDIO - 1, String(String(menuMgr.currentMenu()->getItemIndex() + 1) + "/" + String(menuMgr.currentMenu()->getItemCount())).c_str());  // Index
}

void display_draw() {
  tpDisplay.start();

  u8g2.firstPage();
  do {
    if (menuMgr.isActive()) {
      display_draw_menu();
    } else display_draw_home();

    // Show popup regardless of menu
    if(popup_show)
    {      
      int len = u8g2.getStrWidth(popup_message.c_str());
      
      u8g2.setDrawColor(0);
      u8g2.drawBox(POSX_CENTER - (len / 2), POSY_CENTER -10 , len + 10, 16);
      u8g2.setDrawColor(1);
      u8g2.drawFrame(POSX_CENTER - (len / 2), POSY_CENTER -10 , len + 10, 16);
      u8g2.drawStr(POSX_CENTER - (len / 2) + 5, POSY_CENTER, (popup_message).c_str());
    }

  } while (u8g2.nextPage());

  tpDisplay.stop();
}

// Draws the boot screen with boot log. Called from logger.
void display_draw_startup() {
  int bootup_pct = constrain(((double)bootlog_cnt / (double)BOOTLOG_STEPS) * 100, 0, 100);
  Serial.println("pct: " + String(bootup_pct));

  // Draw last x bootlog lines
  int lines_max = 6;
  int lines_start = bootlog_cnt - lines_max;
  if (lines_start < 0) lines_start = 0;

  u8g2.firstPage();
  do {
    // Draw header
    u8g2.setFont(u8g2_font_tenfatguys_t_all);
    u8g2.drawStr(2, 12, "KitchenRadio 3");

    // Draw startup percentage bar
    u8g2.drawFrame(144, 2, 80, 10);
    u8g2.drawBox(144, 2, map(bootup_pct, 0, 100, 0, 80), 10);

    // Draw startup percentage
    u8g2.setFont(FONT_AUDIO);
    u8g2.drawStr(232, 10, String(String(bootup_pct) + "%").c_str());

    u8g2.setFont(FONT_BOOTLOG);

    int a = 0;
    for (int i = lines_start; i < bootlog_cnt; i++) {
      u8g2.drawStr(4, 22 + (a++) * 8, bootlog[i].c_str());
    }

  } while (u8g2.nextPage());
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
  information.system.display_brightness = map(information.system.ldr, 0, 100, settings.display.brightness_min, settings.display.brightness_max);

  display_set_brightness(information.system.display_brightness);
}

void ticker_popup_cb()
{
  popup_show = false;
}

void display_popup(String message, int length = 3000)
{
  ticker_popup.once_ms(5000, ticker_popup_cb);
  
  LOGG_DEBUG("Popup!");
  //flags.main.displayRedraw = true;
  flags.tickers.displayrefresh = true;
  popup_message = message;
  popup_show = true;
}

// ===  Custom info items ===

// Smiley icons
void display_draw_custominfo_smiley() {
  static int x = 0;
  static int glyph = 48;
  if(x++ > WIDTH_SCREEN)
  {
    x = 0;
    glyph = random(48, 688);
  }
  //uint16_t rand = map(information.minute, 0, 59, 48, 688);  //random(48, 688);
  u8g2.setFont(u8g2_font_streamline_all_t);

  u8g2.drawGlyph(x, 30, glyph);
 
}

// System  info overview
void display_draw_systeminfo_overview() {
  u8g2.setFont(FONT_S);
   
  u8g2.drawStr(10, 12, String(settings.deviceName).c_str());

  u8g2.drawStr(10, 22, "IP: ");
  u8g2.drawStr(70, 22, information.system.ipAddress.c_str());
  u8g2.drawStr(10, 32, "WiFi RSSI:");
  u8g2.drawStr(70, 32, (String(information.system.wifiRSSI) + " dBm").c_str());

  if (information.audioPlayer.soundMode == BLUETOOTH) {
    u8g2.drawStr(10, 42, "BT RSSI:");
    u8g2.drawStr(70, 42, (String(information.audioPlayer.bluetoothRSSI) + " dBm").c_str());  // Show bluetooth RSSI
  } else if (information.audioPlayer.soundMode == WEBRADIO) {
    u8g2.drawStr(10, 42, "Buffer:");
    u8g2.drawStr(70, 42, (String(information.webRadio.bytesAvailable / 1024) + " kB").c_str());
  }

  u8g2.drawStr(150, 12, "Version:");
  u8g2.drawStr(200, 12, KR_VERSION);
  u8g2.drawStr(200, 22, information.system.compilationDateTime.c_str());
  u8g2.drawStr(150, 32, "Uptime:");
  u8g2.drawStr(200, 32, time_convert(information.system.uptimeSeconds).c_str());  
}

// System  info advanced
void display_draw_systeminfo_advanced() {
  u8g2.setFont(FONT_S);
  
  u8g2.drawStr(10, 12, "Core Temp:");
  u8g2.drawStr(70, 12, (String(information.system.coreTemperature) + " 'C").c_str());
  u8g2.drawStr(10, 22, "Rst reason:");
  u8g2.drawStr(70, 22, (String(information.system.lastResetReason)).c_str());
  u8g2.drawStr(10, 32, "Underruns:");
  u8g2.drawStr(70, 32, (String(information.webRadio.cntUnderruns)).c_str());

  u8g2.drawStr(150, 12, "Amb.light:");
  u8g2.drawStr(200, 12, (String(information.system.ldr) + "%").c_str());
  u8g2.drawStr(150, 22, "Disp.bright:");
  u8g2.drawStr(200, 22, (String(information.system.display_brightness) + "%").c_str());

}

// Weather info
void display_draw_custominfo_weather() {
  u8g2.setFont(FONT_S);
  u8g2.drawStr(10, 12, "Wind:");
  u8g2.drawStr(70, 12, (String(information.weather.windSpeedKmh, 1) + "km/h").c_str());
  u8g2.drawStr(10, 22, "Temperature:");
  u8g2.drawStr(70, 22, (String(information.weather.temperature, 1) + " 'C").c_str());
  u8g2.drawStr(10, 32, "Feels like:");
  u8g2.drawStr(70, 32, (String(information.weather.temperature_feelslike, 1) + " 'C").c_str());
  u8g2.drawStr(10, 42, "Humidity:");
  u8g2.drawStr(70, 42, (String(information.weather.humidity) + "%").c_str());

  u8g2.drawStr(150, 12, "Pressure:");
  u8g2.drawStr(200, 12, (String(information.weather.pressure) + " hPa").c_str());
  u8g2.drawStr(150, 22, "Sunrise:");
  u8g2.drawStr(200, 22, (String(information.weather.sunrise_str)).c_str());
  u8g2.drawStr(150, 32, "Sunset:");
  u8g2.drawStr(200, 32, (String(information.weather.sunset_str)).c_str());
}

// Weather forecast hourly
void display_draw_weather_forecast_hourly() {
  u8g2.setFont(FONT_S);
  int hours = 4;
  for(int i = 0; i < hours; i++)
  {
    u8g2.drawStr(10, 12 + (i*10), (String(information.weather.forecast_1h_hour[i]) + ":00").c_str());
    
    u8g2.drawStr(40, 12 + (i*10), (String(information.weather.forecast_1h_description[i]).c_str()));
    u8g2.drawStr(130, 12 + (i*10), (String(information.weather.forecast_1h_temp[i], 1) + " 'C").c_str());
    u8g2.drawStr(180, 12 + (i*10), (String(information.weather.forecast_1h_windspeed_bft[i])).c_str());
    display_draw_triangle_rotated(192, 8 + (i*10), information.weather.forecast_1h_winddir[i]);
  }
}

// Weather forecast daily
void display_draw_weather_forecast_daily() {
  u8g2.setFont(FONT_S);
  
  int days = 4;
  for(int i = 0; i < days; i++)
  {
    u8g2.drawStr(10, 12 + (i*10), (String(information.weather.forecast_1d_day[i])).c_str());
    
    u8g2.drawStr(40, 12 + (i*10), (String(information.weather.forecast_1d_description[i]).c_str()));
    u8g2.drawStr(130, 12 + (i*10), (String(information.weather.forecast_1d_temp[i], 1) + " 'C").c_str());
    u8g2.drawStr(180, 12 + (i*10), (String(information.weather.forecast_1d_windspeed_bft[i])).c_str());
    display_draw_triangle_rotated(192, 8 + (i*10), information.weather.forecast_1d_winddir[i]);
  }
}


void display_draw_triangle_rotated(int cx, int cy, float angle)
{
  // Turn around
    if(angle < 180) angle += 180;
    else angle -= 180;
    float a = angle * PI / 180.0;

    // Original triangle vertices, relative to center
    float x1 =  0, y1 = -4;  // top
    float x2 = -2, y2 =  4;  // bottom-left
    float x3 =  2, y3 =  4;  // bottom-right

    // Rotate + translate
    int rx1 = cx + x1 * cos(a) - y1 * sin(a);
    int ry1 = cy + x1 * sin(a) + y1 * cos(a);

    int rx2 = cx + x2 * cos(a) - y2 * sin(a);
    int ry2 = cy + x2 * sin(a) + y2 * cos(a);

    int rx3 = cx + x3 * cos(a) - y3 * sin(a);
    int ry3 = cy + x3 * sin(a) + y3 * cos(a);

    u8g2.drawTriangle(rx1, ry1, rx2, ry2, rx3, ry3);
}

void display_reset_scroll() {
  display_audio_title_scroll_offset = 0;
  display_audio_title_scroll_dir = 0;
}

void display_update_scroll_offset() {
#define BOX_WIDTH (224 - POSX_AUDIO)
  if (display_audio_title_width < BOX_WIDTH) {
    display_audio_title_scroll_offset = 0;
    return;
  }

  if (display_audio_title_scroll_dir) {
    display_audio_title_scroll_offset += 2;
  } else
    display_audio_title_scroll_offset -= 2;

  // Reverse
  if (display_audio_title_scroll_offset > 2)
    display_audio_title_scroll_dir = !display_audio_title_scroll_dir;

  else if (display_audio_title_scroll_offset < ((BOX_WIDTH - display_audio_title_width) - 3))
    display_audio_title_scroll_dir = !display_audio_title_scroll_dir;
}