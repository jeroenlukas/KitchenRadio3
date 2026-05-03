#include <Arduino.h>
#include <Ticker.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

#include "../configuration/Config.h"
#include "../system/Logger.h"
#include "../information/Information.h"

#include "Lamp.h"
#include "Frontpanel.h"

void lamp_update();

const uint16_t PixelCount = CONFIG_LED_RING_NUM_LEDS; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = CONFIG_PIN_LED_RING;  // make sure to set this to the correct pin, ignored for Esp8266

// NOTE: Please use the CORE3 branch of the NeoPixelBus library! This is compatible with Arduino Core 3

Ticker ticker_effect_ref;
const float MaxLightness = 0.2f; // max lightness at the head of the tail (0.5f is full bright)

NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);


void ticker_effect()
{
  switch(information.lamp.effect_type)
  {
    case EFFECT_NONE:
      // Do nothing
      break;
    case EFFECT_RAINBOW:
      {
          // Hue fade
          static float hue = 0.0;
          
          if((hue += 0.005) > 1.0)
          {
              hue = 0.0;
          }

          HslColor hsl(hue, 1.0, information.lamp.lightness);
          
          for(int i = 0; i < CONFIG_LED_RING_NUM_LEDS; i++)
          {
              strip.SetPixelColor(i, hsl);
          }
          strip.Show();
      }
      break;
    case EFFECT_PULSE:
      {
          // Brightness sawtooth                    
          static float lightness = 0.0;

          if((lightness += 0.005) > 0.5)
          {
              lightness = 0.0;
          }
          HslColor hsl(information.lamp.hue, information.lamp.saturation, lightness);
          
          for(int i = 0; i < CONFIG_LED_RING_NUM_LEDS; i++)
          {
              strip.SetPixelColor(i, hsl);
          }
          strip.Show();
      }
      break;
    case EFFECT_WHEEL:
      { 
        static int a = 0;
        HslColor hsl(information.lamp.hue, information.lamp.saturation, information.lamp.lightness);
        HslColor hslOff(0.0,0.0,0.0);

        if(a++ >= CONFIG_LED_RING_NUM_LEDS)
          a = 0;

        for(int i = 0; i < CONFIG_LED_RING_NUM_LEDS; i++)
        {
          if(i == a)
          {
            hsl.L = information.lamp.lightness; // Reset brightness
            strip.SetPixelColor(i, hsl);   
          }
          else if(i == a-1)
          {
            hsl.L = information.lamp.lightness * 0.3; 
            strip.SetPixelColor(i, hsl);   
          }
          else if(i == a-2)
          {
            hsl.L = information.lamp.lightness * 0.1;
            strip.SetPixelColor(i, hsl);   
          }
          else if(i == a-3)
          {
            hsl.L = information.lamp.lightness * 0.05; 
            strip.SetPixelColor(i, hsl);   
          }
          

          else 
            strip.SetPixelColor(i, hslOff);
          
        }

          
        strip.Show();
      }
      break;
    default:
      break;
  }
}

void lamp_init()
{
    // this resets all the neopixels to an off state
    strip.Begin();
    strip.Show();

    ticker_effect_ref.attach(0.1, ticker_effect);
    
    information.lamp.effect_type = EFFECT_NONE; // No effect
    information.lamp.effect_speed = 100;

    information.lamp.state = false;
    information.lamp.hue = 0.2;
    information.lamp.saturation = 1.0;
    information.lamp.lightness = 0.3;
    
}


void lamp_update()
{
    RgbColor rgb(0,0,0);
    HslColor hsl(rgb);

    hsl.H = information.lamp.hue;
    hsl.S = information.lamp.saturation;

    if(information.lamp.state)
        hsl.L = information.lamp.lightness;
    else hsl.L = 0.0;

    LOGG_DEBUG("xxx");

    for(int i = 0; i < CONFIG_LED_RING_NUM_LEDS; i++)
    {
        strip.SetPixelColor(i, hsl);
    }
    strip.Show();

    frontpanel_leds_handle();
}

void lamp_setstate(bool state)
{
  information.lamp.state = state;

  lamp_update();
}

void lamp_toggle()
{
  information.lamp.state = !information.lamp.state;

  lamp_update();
}

// TODO deprecate
void lamp_on()
{
  information.lamp.state = true;
  lamp_update();
}

// TODO deprecate
void lamp_off()
{
  information.lamp.state = false;
  lamp_update();
}

void lamp_handle()
{

}

// H, S values (0.0 - 1.0)
// L should be limited to between (0.0 - 0.5)
void lamp_sethue(float hue)
{    
  LOGG_DEBUG("Setting hue to " + String(hue));
  information.lamp.hue = constrain(hue, 0.0, 1.0);
  if(information.lamp.effect_type == EFFECT_NONE)
    lamp_update();    
}

void lamp_setsaturation(float saturation)
{
  information.lamp.saturation = constrain(saturation, 0.0, 1.0);
  if(information.lamp.effect_type == EFFECT_NONE)
    lamp_update();
}

void lamp_setlightness(float lightness)
{    
  information.lamp.lightness = constrain(lightness, 0.0, 0.5);
  if(information.lamp.effect_type == EFFECT_NONE)
    lamp_update();
}

void lamp_seteffecttype(lampEffectType_t effect)
{
    information.lamp.effect_type = effect;     

}

void lamp_seteffectspeed(int speed)
{
    information.lamp.effect_speed = speed;
    ticker_effect_ref.restart_ms(speed);
}