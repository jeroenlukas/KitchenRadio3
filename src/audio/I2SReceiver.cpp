#include "../configuration/Config.h"

#include <Arduino.h>
#include <AudioTools.h>
//#include <driver/i2s.h>

#include "Audioplayer.h"

#include "../information/Information.h"

HardwareSerial serial_bt(2);
uint8_t bt_wav_header[44] = {
    0x52, 0x49, 0x46, 0x46, // RIFF
    0xFF, 0xFF, 0xFF, 0xFF, // size
    0x57, 0x41, 0x56, 0x45, // WAVE
    0x66, 0x6d, 0x74, 0x20, // fmt
    0x10, 0x00, 0x00, 0x00, // subchunk1size
    0x01, 0x00,             // audio format - pcm
    0x02, 0x00,             // numof channels
    0x44, 0xac, 0x00, 0x00, //, //samplerate 44k1: 0x44, 0xac, 0x00, 0x00       48k: 48000: 0x80, 0xbb, 0x00, 0x00,
    0x10, 0xb1, 0x02, 0x00, //byterate
    0x04, 0x00,             // blockalign
    0x10, 0x00,             // bits per sample - 16
    0x64, 0x61, 0x74, 0x61, // subchunk3id -"data"
    0xFF, 0xFF, 0xFF, 0xFF  // subchunk3size (endless)
};

I2SStream i2sStream;
StreamCopy i2scopier(vs1053, i2sStream); 

void i2sreceiver_init()
{
    Serial.println("I2SReceiver init");

    auto config = i2sStream.defaultConfig(RX_MODE);
    config.i2s_format = I2S_STD_FORMAT;//I2S_STD_FORMAT; // if quality is bad change to I2S_LSB_FORMAT https://github.com/pschatzmann/arduino-audio-tools/issues/23
    config.sample_rate = 44100;
    
    config.is_master = false;

    config.channels = 2;
    config.bits_per_sample = 16;
    config.pin_bck = CONFIG_PIN_SLAVEI2S_SCK;
    config.pin_ws = CONFIG_PIN_SLAVEI2S_WS;
    config.pin_data_rx = CONFIG_PIN_SLAVEI2S_SD;
    config.buffer_count = 8;
    config.buffer_size = 512;
    config.use_apll = false;
    i2sStream.begin(config);
    Serial.println("I2S started");

    // Init UART

    serial_bt.begin(115200, SERIAL_8N1, CONFIG_PIN_UART_BT_RX, CONFIG_PIN_UART_BT_TX);
    serial_bt.print("AT+END" + '\n');

}

void i2sreceiver_handle()
{
    if(information.audioPlayer.soundMode == BLUETOOTH)
    {

        i2scopier.copy();
    }

    if(serial_bt.available() > 0)
    {
        Serial.println("avail!");
        String str = serial_bt.readStringUntil('\n');      
    
        Serial.println("Recv: "  + str);
        // slavei2s_command_parse(str);
    }
}

void i2sreceiver_start()
{
    //copier.begin();
    Serial.println("i2sreceiver_start!");
    i2scopier.begin();
    vs1053.write(bt_wav_header, 44);
    serial_bt.print("AT+START" + '\n');
}

void i2sreceiver_stop()
{
    Serial.println("i2sreceiver_stop!");
    i2scopier.end();
    serial_bt.print("AT+END" + '\n');
}

void i2sreceiver_send(String str)
{
    Serial.println("Sending: " + str);
    serial_bt.print(str + '\n');

}