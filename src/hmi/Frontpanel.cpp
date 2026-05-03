#include <Arduino.h>
#include "../configuration/Config.h"
#include "../information/Information.h"

#include <RotaryEncoder.h>
#include <Adafruit_MCP23X17.h>

#include "../system/Logger.h"
#include "../events/Flags.h"

RotaryEncoder encoder1(CONFIG_PIN_ROTARY1_A, CONFIG_PIN_ROTARY1_B, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoder2(CONFIG_PIN_ROTARY2_A, CONFIG_PIN_ROTARY2_B, RotaryEncoder::LatchMode::FOUR3);

Adafruit_MCP23X17 mcp;

void frontpanel_encoders_read();
void frontpanel_nightmode_handle();
void frontpanel_leds_handle();
void front_buttons_read();
void front_i2c_ping();
void front_ldr_read();

// -----------------------------------------------------------------------

void frontpanel_begin()
{
    //LOGG_INFO("")
    // LDR
    pinMode(CONFIG_PIN_LDR, INPUT);

    // MCP interrupts
    pinMode(CONFIG_PIN_MCP_INTA, INPUT);
    pinMode(CONFIG_PIN_MCP_INTB, INPUT);

    Wire.setPins(CONFIG_PIN_SDA, CONFIG_PIN_SCL);
    Wire.begin();
    
    mcp.begin_I2C();    

    // Buttons
    mcp.pinMode(CONFIG_PIN_MCP_BTN_OFF, INPUT_PULLUP);
    mcp.pinMode(CONFIG_PIN_MCP_BTN_WEBRADIO, INPUT_PULLUP);
    mcp.pinMode(CONFIG_PIN_MCP_BTN_BLUETOOTH, INPUT_PULLUP);
    mcp.pinMode(CONFIG_PIN_MCP_BTN_SYSTEM, INPUT_PULLUP);
    mcp.pinMode(CONFIG_PIN_MCP_BTN_ALARM, INPUT_PULLUP);
    mcp.pinMode(CONFIG_PIN_MCP_BTN_LAMP, INPUT_PULLUP);
    mcp.pinMode(CONFIG_PIN_MCP_BTN_ENC1, INPUT_PULLUP);
    mcp.pinMode(CONFIG_PIN_MCP_BTN_ENC2, INPUT_PULLUP);

    // Interrupts are disabled by default
    mcp.setupInterruptPin(CONFIG_PIN_MCP_BTN_OFF, CHANGE);
    mcp.setupInterruptPin(CONFIG_PIN_MCP_BTN_WEBRADIO, CHANGE);
    mcp.setupInterruptPin(CONFIG_PIN_MCP_BTN_BLUETOOTH, CHANGE);
    mcp.setupInterruptPin(CONFIG_PIN_MCP_BTN_SYSTEM, CHANGE);
    mcp.setupInterruptPin(CONFIG_PIN_MCP_BTN_ALARM, CHANGE);
    mcp.setupInterruptPin(CONFIG_PIN_MCP_BTN_LAMP, CHANGE);
    mcp.setupInterruptPin(CONFIG_PIN_MCP_BTN_ENC1, CHANGE);
    mcp.setupInterruptPin(CONFIG_PIN_MCP_BTN_ENC2, CHANGE);
    mcp.clearInterrupts();

    // LEDs
    mcp.pinMode(CONFIG_PIN_MCP_LED_WEBRADIO, OUTPUT);
    mcp.pinMode(CONFIG_PIN_MCP_LED_BLUETOOTH, OUTPUT);    
    mcp.pinMode(CONFIG_PIN_MCP_LED_ALARM, OUTPUT);
    mcp.pinMode(CONFIG_PIN_MCP_LED_LAMP, OUTPUT);

    // All LEDs are on during startup
    mcp.digitalWrite(CONFIG_PIN_MCP_LED_WEBRADIO, HIGH);    
    mcp.digitalWrite(CONFIG_PIN_MCP_LED_BLUETOOTH, HIGH);
    mcp.digitalWrite(CONFIG_PIN_MCP_LED_ALARM, HIGH);
    mcp.digitalWrite(CONFIG_PIN_MCP_LED_LAMP, HIGH);

    // Buzzer
    pinMode(CONFIG_PIN_BUZZER, OUTPUT);
    
}

void frontpanel_handle()
{
  frontpanel_encoders_read();  
  frontpanel_nightmode_handle();
}

void frontpanel_ldr_read()
{
    uint16_t an = analogRead(CONFIG_PIN_LDR);
    uint16_t adc = 4095 - an;

    information.system.ldr = map(adc, 0, 4095, 0, 100);    
}

void frontpanel_nightmode_handle()
{
    
    if((information.system.ldr < CONF_NIGHTMODE_LEVEL) && (!information.system.nightmode))
    {
        LOGG_DEBUG("Entering night mode!");
        information.system.nightmode = true;
        frontpanel_leds_handle();
    }
    else if ((information.system.ldr > CONF_NIGHTMODE_LEVEL) && (information.system.nightmode))
    {
        LOGG_DEBUG("Exiting night mode!");
        information.system.nightmode = false;
        frontpanel_leds_handle();
    }
}

// Should be called when LEDs should be updated, for example when changing audio mode.
void frontpanel_leds_handle()
{
    LOGG_DEBUG("Settings LEDs");

    // Turn off  all leds when in nightmode
    if(information.system.nightmode)
    {
        mcp.digitalWrite(CONFIG_PIN_MCP_LED_WEBRADIO, LOW);
        mcp.digitalWrite(CONFIG_PIN_MCP_LED_BLUETOOTH, LOW);
        mcp.digitalWrite(CONFIG_PIN_MCP_LED_ALARM, LOW);
        mcp.digitalWrite(CONFIG_PIN_MCP_LED_LAMP, LOW);
    }
    else
    {
        //if(information.audioPlayer.soundMode == WEBRADIO)
        mcp.digitalWrite(CONFIG_PIN_MCP_LED_WEBRADIO, information.audioPlayer.soundMode == WEBRADIO ? true : false);
        mcp.digitalWrite(CONFIG_PIN_MCP_LED_BLUETOOTH, information.audioPlayer.soundMode == BLUETOOTH ? true : false);
        mcp.digitalWrite(CONFIG_PIN_MCP_LED_LAMP, information.lamp.state);
    }
}

// Read buttons. Should be read every 100ms
void frontpanel_buttons_read()
{
    static uint8_t lastbutton = 0xFF;
    static uint32_t lastpressdown = 0;


    int mcp_inta = !digitalRead(CONFIG_PIN_MCP_INTA); // Interrupt for encoder switches
    int mcp_intb = !digitalRead(CONFIG_PIN_MCP_INTB); // Interrupt
    
    // Handle long press stuff
    if((lastpressdown > 0) && (lastbutton != 0xFF))
    {
        if((millis() - lastpressdown) > 1000)
        {
            LOGG_DEBUG("LONG press " + String(lastbutton));
            lastpressdown = 0;

            switch(lastbutton)
                {
                    case CONFIG_PIN_MCP_BTN_OFF:
                        flags.frontPanel.buttonOffLongPressed = true;
                        break;
                    case CONFIG_PIN_MCP_BTN_WEBRADIO:
                        flags.frontPanel.buttonRadioLongPressed = true;
                        break;
                    case CONFIG_PIN_MCP_BTN_BLUETOOTH:
                        flags.frontPanel.buttonBluetoothLongPressed = true;
                        break;
                    case CONFIG_PIN_MCP_BTN_SYSTEM:
                        flags.frontPanel.buttonSystemLongPressed = true;
                        break;
                    case CONFIG_PIN_MCP_BTN_ALARM:
                        flags.frontPanel.buttonAlarmLongPressed = true;
                        break;
                    case CONFIG_PIN_MCP_BTN_LAMP:
                        flags.frontPanel.buttonLampLongPressed = true;
                        break;    
                    case CONFIG_PIN_MCP_BTN_ENC1:
                        flags.frontPanel.encoder1ButtonLongPressed = true;
                        break;
                    case CONFIG_PIN_MCP_BTN_ENC2:
                        flags.frontPanel.encoder2ButtonLongPressed = true;
                        break;                
                    default:
                        break;

                }

             lastbutton = 0xff;
        }
    }

    if(mcp_inta || mcp_intb) // Button was pushed or released
    {        
        uint8_t button = mcp.getLastInterruptPin();
        uint16_t value = !((mcp.getCapturedInterrupt() >> button) & 1);
   
        
        if(value) // Button was pushed
        {
            lastbutton = button;
            lastpressdown = millis(); // button is pushed            
        }
        else  // Button was released
        {
            if(button == lastbutton) 
            {
                LOGG_DEBUG("Short press " + String(button));
                lastpressdown = 0; // 'reset' 

                switch(button)
                {
                    case CONFIG_PIN_MCP_BTN_OFF:
                        flags.frontPanel.buttonOffPressed = true;
                        break;
                    case CONFIG_PIN_MCP_BTN_WEBRADIO:
                        flags.frontPanel.buttonRadioPressed = true;
                        break;
                    case CONFIG_PIN_MCP_BTN_BLUETOOTH:
                        flags.frontPanel.buttonBluetoothPressed = true;
                        break;
                    case CONFIG_PIN_MCP_BTN_SYSTEM:
                        flags.frontPanel.buttonSystemPressed = true;
                        break;
                    case CONFIG_PIN_MCP_BTN_ALARM:
                        flags.frontPanel.buttonAlarmPressed = true;
                        break;
                    case CONFIG_PIN_MCP_BTN_LAMP:
                        flags.frontPanel.buttonLampPressed = true;
                        break;    
                    case CONFIG_PIN_MCP_BTN_ENC1:
                        flags.frontPanel.encoder1ButtonPressed = true;
                        break;
                    case CONFIG_PIN_MCP_BTN_ENC2:
                        flags.frontPanel.encoder2ButtonPressed = true;
                        break;                
                    default:
                        break;

                }
            }
        }
                
        mcp.clearInterrupts(); 

        flags.frontPanel.buttonAnyPressed = true;
    }
}

void frontpanel_encoders_read()
{
    static int pos1 = 0;
    encoder1.tick();

    int newPos1 = encoder1.getPosition();
    if (pos1 != newPos1)
    {
        if ((int)(encoder1.getDirection()) == -1)
        {
            flags.frontPanel.encoder1TurnRight = true;
           LOGG_DEBUG("1 right");
        }
        else
        {
            flags.frontPanel.encoder1TurnLeft = true;            
            LOGG_DEBUG("1 left");
        }
        pos1 = newPos1;
        flags.frontPanel.buttonAnyPressed = true;
    }

    static int pos2 = 0;
    encoder2.tick();

    int newPos2 = encoder2.getPosition();
    if (pos2 != newPos2)
    {
        if ((int)(encoder2.getDirection()) == 1)
        {
            flags.frontPanel.encoder2TurnRight = true;
            LOGG_DEBUG("2 right");
        }
        else
        {
            flags.frontPanel.encoder2TurnLeft = true;           
            LOGG_DEBUG("2 left");
        }
        flags.frontPanel.buttonAnyPressed = true;
        pos2 = newPos2;
    }
}

void frontpanel_i2c_ping()
{
    LOGG_INFO("Wire ping");
    delay(100);

    byte error, address;
    int nDevices;
    LOGG_INFO("Scanning...");
    nDevices = 0;
    for(address = 1; address < 127; address++ ) 
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
            LOGG_INFO("I2C device found at address " + address);
            nDevices++;
        }
        else if (error==4) 
        {
            LOGG_INFO("Unknown error at address " + address);
        }    
    }
    if (nDevices == 0) 
    {
        LOGG_INFO("No I2C devices found");
    }
    else 
    {
        LOGG_INFO("Done");
    }
}

void frontpanel_buzzer_beep(uint16_t duration)
{
    digitalWrite(CONFIG_PIN_BUZZER, HIGH);

    delay(duration);

    digitalWrite(CONFIG_PIN_BUZZER, LOW);
}