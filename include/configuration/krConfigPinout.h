#ifndef KR_CONFIG_PINOUT_H
#define KR_CONFIG_PINOUT_H

#define CONFIG_PIN_HSPI_SCK        12 
#define CONFIG_PIN_HSPI_MISO       13
#define CONFIG_PIN_HSPI_MOSI       11
#define CONFIG_PIN_HSPI_CS         48
#define CONFIG_PIN_HSPI_DC         10

// VS1053 board (SPI connected in a standard way)
#define CONFIG_PIN_VS1053_CS       14
#define CONFIG_PIN_VS1053_DCS      47
#define CONFIG_PIN_VS1053_DREQ     21

// MCP I/O
// https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library/tree/master?tab=readme-ov-file#pin-addressing
#define CONFIG_PIN_MCP_INTA            15
#define CONFIG_PIN_MCP_INTB            16

// LEDs
#define CONFIG_PIN_MCP_LED_WEBRADIO   3
#define CONFIG_PIN_MCP_LED_BLUETOOTH  2
#define CONFIG_PIN_MCP_LED_ALARM      1
#define CONFIG_PIN_MCP_LED_LAMP       0

// Buttons
#define CONFIG_PIN_MCP_BTN_OFF        8
#define CONFIG_PIN_MCP_BTN_WEBRADIO   9
#define CONFIG_PIN_MCP_BTN_BLUETOOTH   10
#define CONFIG_PIN_MCP_BTN_ALARM       11
#define CONFIG_PIN_MCP_BTN_LAMP        12
#define CONFIG_PIN_MCP_BTN_SYSTEM      13
#define CONFIG_PIN_MCP_BTN_ENC1        4
#define CONFIG_PIN_MCP_BTN_ENC2        5

// Encoders
#define CONFIG_PIN_ROTARY1_A        7 
#define CONFIG_PIN_ROTARY1_B        6 
#define CONFIG_PIN_ROTARY2_A        5
#define CONFIG_PIN_ROTARY2_B        4

// I2C
#define CONFIG_PIN_SCL         1
#define CONFIG_PIN_SDA         2

// LDR
#define CONFIG_PIN_LDR             17

// LED ring
#define CONFIG_PIN_LED_RING    38

// Power amp
#define CONFIG_PIN_PA_MUTE     39

// Buzzer
#define CONFIG_PIN_BUZZER      18

// I2S
#define CONFIG_PIN_SLAVEI2S_SCK     42
#define CONFIG_PIN_SLAVEI2S_SD      41
#define CONFIG_PIN_SLAVEI2S_WS      40
#define CONFIG_PIN_UART_BT_TX  43
#define CONFIG_PIN_UART_BT_RX  44


#endif