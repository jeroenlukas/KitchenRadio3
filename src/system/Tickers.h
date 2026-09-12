#ifndef KR_TICKERS_H
#define KR_TICKERS_H

void tickers_init();
void tickers_handle();

void tickers_userinput_reset();
void tickers_displayrefresh_setinterval(uint32_t ms);

#endif