#include <Arduino.h>
#include <SPI.h>

#include <U8g2lib.h>

#include "configuration/krConfig.h"
#include "information/krInformation.h"

U8G2_SSD1322_NHD_256X64_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ CONFIG_PIN_HSPI_CS, /* dc=*/ CONFIG_PIN_HSPI_DC, /* reset=*/ 9);	// Enable U8G2_16BIT in u8g2.h

SPIClass *hspi = NULL;

void display_begin()
{
    hspi = new SPIClass(HSPI);
    hspi->begin(CONFIG_PIN_HSPI_SCK, CONFIG_PIN_HSPI_MISO, CONFIG_PIN_HSPI_MOSI, CONFIG_PIN_HSPI_CS);  
    u8g2.begin();
}

void display_draw()
{
    u8g2.firstPage();
    do 
    {      
      u8g2.setFont(u8g2_font_minicute_tr);
      
      u8g2.drawStr(10, 26, String("millis: " + String(millis()/1000) ).c_str());
      u8g2.drawStr(10, 36, String("Radio: " + information.webRadio.title).c_str());
      u8g2.drawLine(0, 48, 256, 48);      
      
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