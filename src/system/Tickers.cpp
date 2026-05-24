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
#include "Logger.h"

#define ONEMINUTE (60*1000)
#define ONESECOND 1000

// Ticker callbacks
void ticker_30m_cb();
void ticker_1s_cb();
void ticker_100ms_cb();
void ticker_userinput_cb();
void ticker_displayrefresh_cb();

// Interval tickers
TickTwo ticker_30m(ticker_30m_cb, ONEMINUTE * 30);
TickTwo ticker_1s(ticker_1s_cb, ONESECOND);
TickTwo ticker_100ms(ticker_100ms_cb, 100);

// Special tickers
TickTwo ticker_userinput(ticker_userinput_cb, CONF_MENU_RETURN_HOME_MS);
TickTwo ticker_displayrefresh(ticker_displayrefresh_cb, CONF_DISPLAYREFRESH_MS);

// Executed every 30 minutes
void ticker_30m_cb()
{
  flags.tickers.passed30min = true;
}

// Executed every second
void ticker_1s_cb()
{
  flags.tickers.passed1s = true;
}

// Executed every 100ms
void ticker_100ms_cb()
{
  frontpanel_buttons_read();
  display_update_scroll_offset();
  webradio_calculatebufferpct();
}

void ticker_displayrefresh_cb()
{
  flags.tickers.displayrefresh = true;
}

void ticker_userinput_cb()
{
  flags.tickers.userinput = true;
}


// ---

void tickers_init()
{
  LOGG_DEBUG("Tickers init");
  
  ticker_30m.start();
  ticker_1s.start();
  ticker_100ms.start();    

  ticker_userinput.start();
  ticker_displayrefresh.start();
}

void tickers_handle()
{
  ticker_30m.update();
  ticker_1s.update();
  ticker_100ms.update();
  
  ticker_userinput.update();
  ticker_displayrefresh.update();
}

void tickers_userinput_reset()
{
  // Restart the ticker
  ticker_userinput.stop();
  ticker_userinput.start();
}