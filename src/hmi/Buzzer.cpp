#include <Arduino.h>
#include "../configuration/Config.h"
#include "../information/Information.h"
//#include <TickTwo.h>
#include <Ticker.h>

#include "../system/Logger.h"
#include "../events/Flags.h"

void ticker_buzzer_cb();

//TickTwo ticker_buzzer(ticker_buzzer_cb, 100, 0 , MICROS_MICROS);
Ticker ticker_buzzer;

void buzzer_init()
{  
  pinMode(CONFIG_PIN_BUZZER, OUTPUT);

  //ticker_buzzer.attach(0.1, ticker_buzzer_cb);
}



void ticker_buzzer_cb()
{
  // Turn off buzzer
  digitalWrite(CONFIG_PIN_BUZZER, LOW);

  LOGG_DEBUG("END buzzer");

  //ticker_buzzer.stop();

}
/*
void buzzer_handle()
{
  ticker_buzzer.update();
}*/

// Beep the buzzer using a timer. duration in microseconds
void buzzer_beep(uint32_t duration_us)
{
    digitalWrite(CONFIG_PIN_BUZZER, HIGH);

    LOGG_DEBUG("RESTART buzzer");
    //delay(duration);
    ticker_buzzer.once_us(duration_us, ticker_buzzer_cb);

    //ticker_buzzer.interval(duration_us);
    //ticker_buzzer.start();

    //ticker_buzzer_ref.start();


   // digitalWrite(CONFIG_PIN_BUZZER, LOW);
}

