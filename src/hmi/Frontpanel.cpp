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
void front_buttons_read();
void front_i2c_ping();

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
    mcp.digitalWrite(CONFIG_PIN_MCP_LED_WEBRADIO, HIGH);    
    mcp.digitalWrite(CONFIG_PIN_MCP_LED_BLUETOOTH, HIGH);
    
}

void frontpanel_handle()
{
  frontpanel_encoders_read();  
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
                        //flags.frontPanel.encoder1ButtonLongPressed = true;
                        break;
                    case CONFIG_PIN_MCP_BTN_ENC2:
                        //flags.frontPanel.encoder2ButtonLongPressed = true;
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
            LOGG_DEBUG("PUSH" );
        }
        else  // Button was released
        {
            LOGG_DEBUG("RELEASE" );
            if(button == lastbutton) 
            {
                LOGG_DEBUG("Short press " + String(button));
                lastpressdown = 0; // 'reset' 

                switch(button)
                {
                    case CONFIG_PIN_MCP_BTN_OFF:
                        LOGG_INFO("Off!!!");
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
            //if (address<16) {
            //  LOG_INFO("0");
            //}
            //Serial.println(address,HEX);
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
