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

void ticker_30m();
void ticker_1s();
void ticker_100ms();
void ticker_userinput();
void ticker_displayrefresh();

TickTwo ticker_30m_ref(ticker_30m, ONEMINUTE * 30);
TickTwo ticker_1s_ref(ticker_1s, ONESECOND);
TickTwo ticker_100ms_ref(ticker_100ms, 100);

TickTwo ticker_userinput_ref(ticker_userinput, CONF_MENU_RETURN_HOME_MS);
TickTwo ticker_displayrefresh_ref(ticker_displayrefresh, CONF_DISPLAYREFRESH_MS);


// Executed every 30 minutes
void ticker_30m()
{
  flags.tickers.passed30min = true;
}

// Executed every second
void ticker_1s()
{
  flags.tickers.passed1s = true;
}

// Executed every 100ms
void ticker_100ms()
{
  frontpanel_buttons_read();
  display_update_scroll_offset();
  webradio_calculatebufferpct();
}

void ticker_displayrefresh()
{
  flags.tickers.displayrefresh = true;
}

void ticker_userinput()
{
  flags.tickers.userinput = true;
}


// ---

void tickers_init()
{
  LOGG_DEBUG("Tickers init");
  
  ticker_30m_ref.start();
  ticker_1s_ref.start();
  ticker_100ms_ref.start();    

  ticker_userinput_ref.start();
  ticker_displayrefresh_ref.start();
}

void tickers_handle()
{
  ticker_30m_ref.update();
  ticker_1s_ref.update();
  ticker_100ms_ref.update();
  
  ticker_userinput_ref.update();
  ticker_displayrefresh_ref.update();
}

void tickers_userinput_reset()
{
  // Restart the ticker
  ticker_userinput_ref.stop();
  ticker_userinput_ref.start();
}