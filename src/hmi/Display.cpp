#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>

#include "../system/Logger.h"
#include "../information/Time.h"
#include "../system/Settings.h"
#include "Menu.h"


U8G2_SSD1322_NHD_256X64_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ CONFIG_PIN_HSPI_CS, /* dc=*/ CONFIG_PIN_HSPI_DC, /* reset=*/ 9);	// Enable U8G2_16BIT in u8g2.h

SPIClass *hspi = NULL;

void display_begin()
{
  LOGG_INFO("Display init");
  hspi = new SPIClass(HSPI);
  hspi->begin(CONFIG_PIN_HSPI_SCK, CONFIG_PIN_HSPI_MISO, CONFIG_PIN_HSPI_MOSI, CONFIG_PIN_HSPI_CS);  
  u8g2.begin();
}

// === Home screen ===
void display_draw_home()
{
  u8g2.setFont(u8g2_font_likeminecraft_te);
            
  u8g2.drawStr(10, 26, String("bytes: " + String(information.webRadio.bytesAvailable) + " underrun: " + String(information.webRadio.cntUnderruns)).c_str());

  // Clock
  u8g2.setCursor(POSX_CLOCK, POSY_CLOCK);
  u8g2.print(u8x8_u8toa(information.hour, 2)); 
  u8g2.drawStr(POSX_CLOCK + 30, POSY_CLOCK - 2, ":");
  u8g2.setCursor(POSX_CLOCK + 39, POSY_CLOCK);
  u8g2.print(u8x8_u8toa(information.minute, 2));
  
  switch(information.audioPlayer.soundMode)
  {
    case OFF:
      u8g2.drawStr(10, 36, "-Off-");
      break;
    case WEBRADIO:
      u8g2.drawStr(10, 36, String("Radio: " + information.webRadio.title).c_str());
      break;
    case BLUETOOTH:
      u8g2.drawStr(10, 36, String("Bt: " + information.audioPlayer.bluetoothTitle).c_str());
      break;
    default:
      break;

  }
  
  
  u8g2.drawLine(0, 48, 256, 48);      

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
    u8g2.firstPage();
    do 
    {      
      if(menuMgr.isActive())
      {
        display_draw_menu();
      }
      else display_draw_home();
      
    } while ( u8g2.nextPage() );

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

// ===  Custom info items ===

// System stats
void display_draw_custominfo_system()
{
  u8g2.setFont(FONT_S);
  u8g2.drawStr(10, 12, String(settings.deviceName).c_str());
  u8g2.drawStr(10, 22, "IP: ");         u8g2.drawStr(70, 22, information.system.ipAddress.c_str());
  u8g2.drawStr(10, 32, "RSSI:");        u8g2.drawStr(70, 32, (String(information.system.wifiRSSI) + " dBm").c_str());      
  u8g2.drawStr(10, 42, "BT RSSI:");     
  if(information.audioPlayer.soundMode == BLUETOOTH)
    u8g2.drawStr(70, 42, (String(information.audioPlayer.bluetoothRSSI) + " dBm").c_str());      
  else
    u8g2.drawStr(70, 42, "N/A");      

  u8g2.drawStr(150, 12, "Uptime:");     u8g2.drawStr(200, 12, time_convert(information.system.uptimeSeconds).c_str());
  u8g2.drawStr(150, 22, "Amb.light:");  u8g2.drawStr(200, 22, (String(information.system.ldr) + "%").c_str());
  u8g2.drawStr(150, 32, "Rst reason:");  u8g2.drawStr(200, 32, (String(information.system.lastResetReason)).c_str());
}

// Smiley icons
void display_draw_custominfo_smiley()
{
  u8g2.setFont(u8g2_font_streamline_all_t);
  u8g2.drawGlyph(80,30,228);
  u8g2.drawGlyph(150,47,592);
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