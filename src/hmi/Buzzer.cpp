#include <Arduino.h>
#include "../configuration/Config.h"
#include "../information/Information.h"
//#include <TickTwo.h>
#include <Ticker.h>

#include "../system/Logger.h"
#include "../events/Flags.h"

void ticker_buzzer_cb();

Ticker ticker_buzzer;

void buzzer_init()
{  
  pinMode(CONFIG_PIN_BUZZER, OUTPUT);
}

void ticker_buzzer_cb()
{
  // Turn off buzzer
  digitalWrite(CONFIG_PIN_BUZZER, LOW);  
}

// Beep the buzzer using a timer. duration in microseconds
void buzzer_beep(uint32_t duration_us)
{
  digitalWrite(CONFIG_PIN_BUZZER, HIGH);

  ticker_buzzer.once_us(duration_us, ticker_buzzer_cb);
}

// Play alarm 
void buzzer_alarm()
{

}