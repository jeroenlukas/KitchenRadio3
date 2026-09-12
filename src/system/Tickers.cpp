#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <TickTwo.h>

#include "Tickers.h"

#include "../events/Flags.h"
#include "../information/Weather.h"
#include "../hmi/Frontpanel.h"
#include "../information/Time.h"
#include "../hmi/Display.h"
#include "../audio/Webradio.h"
#include "../hmi/Alarm.h"
#include "Logger.h"

#define ONESECOND 1000
#define ONEMINUTE (60*1000)
#define ONEHOUR (ONEMINUTE*60)
#define ONEDAY (ONEHOUR*24)

// Ticker callbacks
void ticker_100ms_cb();
void ticker_1s_cb();
void ticker_30m_cb();
void ticker_1h_cb();
void ticker_3h_cb();
void ticker_6h_cb();
void ticker_24h_cb();

void ticker_userinput_menu_cb();
void ticker_userinput_stationselect_cb();
void ticker_displayrefresh_cb();

// Interval tickers
TickTwo ticker_100ms(ticker_100ms_cb, 100);
TickTwo ticker_1s(ticker_1s_cb, ONESECOND);
TickTwo ticker_30m(ticker_30m_cb, ONEMINUTE * 30);
TickTwo ticker_1h(ticker_1h_cb, ONEHOUR);
// Add MILLIS for tickers with over 70 minutes interval!
TickTwo ticker_3h(ticker_3h_cb, ONEHOUR * 3, 0, MILLIS);  
TickTwo ticker_6h(ticker_6h_cb, ONEHOUR * 6, 0, MILLIS);
TickTwo ticker_24h(ticker_24h_cb, ONEHOUR * 24, 0, MILLIS);
        
// Special tickers
TickTwo ticker_userinput_menu(ticker_userinput_menu_cb, CONF_MENU_RETURN_HOME_MS); // Return to home screen after x seconds of no user input
TickTwo ticker_userinput_stationselect(ticker_userinput_stationselect_cb, CONF_MENU_STATIONSELECT_EXIT); // Close station select after x seconds of no user input
TickTwo ticker_displayrefresh(ticker_displayrefresh_cb, CONF_DISPLAYREFRESH_MS);

// Executed every 100ms - does not use flags, execute immediately
void ticker_100ms_cb()
{
  frontpanel_buttons_read();
  display_update_scroll_offset();
  webradio_calculatebufferpct();
  frontpanel_ldr_read();
}

// Executed every second
void ticker_1s_cb(){ flags.tickers.passed1s = true; }

// Executed every 30 minutes
void ticker_30m_cb(){  flags.tickers.passed30min = true;}

// Executed hourly
void ticker_1h_cb(){ flags.tickers.passed1h = true; }
void ticker_3h_cb(){ flags.tickers.passed3h = true; }
void ticker_6h_cb(){ flags.tickers.passed6h = true; }
void ticker_24h_cb(){ flags.tickers.passed24h = true; }

void ticker_displayrefresh_cb(){ flags.tickers.displayrefresh = true; }

void ticker_userinput_menu_cb(){ flags.tickers.userinput_menu = true; }

void ticker_userinput_stationselect_cb(){ flags.tickers.userinput_stationselect = true;}

// ---

void tickers_init()
{
  LOGG_DEBUG("Tickers init");
  
  ticker_100ms.start();    
  ticker_1s.start();
  ticker_30m.start();
  ticker_1h.start();
  ticker_3h.start();
  ticker_6h.start();
  ticker_24h.start();

  ticker_userinput_menu.start();
  ticker_userinput_stationselect.start();
  ticker_displayrefresh.start();
}

void tickers_displayrefresh_setinterval(uint32_t ms)
{
  ticker_displayrefresh.interval(ms);
}

void tickers_handle()
{
  ticker_100ms.update();
  ticker_1s.update();  
  ticker_30m.update();
  ticker_1h.update();
  ticker_3h.update();
  ticker_6h.update();
  ticker_24h.update();
  
  ticker_userinput_menu.update();
  ticker_userinput_stationselect.update();
  ticker_displayrefresh.update();
}

void tickers_userinput_reset()
{
  // Restart the ticker
  ticker_userinput_menu.stop();
  ticker_userinput_menu.start();

  ticker_userinput_stationselect.stop();
  ticker_userinput_stationselect.start();
}