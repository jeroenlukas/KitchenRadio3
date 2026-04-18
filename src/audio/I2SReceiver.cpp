#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <AudioTools.h>
#include "../system/Logger.h"

#include "Audioplayer.h"
#include "../hmi/Display.h"

HardwareSerial serial_bt(2);

uint8_t bt_wav_header[44] = 
{
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

void i2sreceiver_command_parse(String command);
void i2sreceiver_send(String str);

void i2sreceiver_init()
{
    LOGG_INFO("I2SReceiver init");

    auto config = i2sStream.defaultConfig(RX_MODE);
    config.i2s_format = I2S_STD_FORMAT;//I2S_STD_FORMAT; // if quality is bad change to I2S_LSB_FORMAT https://github.com/pschatzmann/arduino-audio-tools/issues/23
    config.sample_rate = 44100;    
    config.is_master = false; // Important - BT module is master
    config.channels = 2;
    config.bits_per_sample = 16;
    config.pin_bck = CONFIG_PIN_SLAVEI2S_SCK;
    config.pin_ws = CONFIG_PIN_SLAVEI2S_WS;
    config.pin_data_rx = CONFIG_PIN_SLAVEI2S_SD;
    config.buffer_count = 8;
    config.buffer_size = 512;
    config.use_apll = false;
    i2sStream.begin(config);
    LOGG_INFO("I2S started");

    // Init UART
    serial_bt.begin(115200, SERIAL_8N1, CONFIG_PIN_UART_BT_RX, CONFIG_PIN_UART_BT_TX);
    i2sreceiver_send("AT+END");

}

void i2sreceiver_handle()
{
    i2scopier.copy();
}



void i2sreceiver_start()
{
    LOGG_DEBUG("i2sreceiver_start!");
    i2sreceiver_send("AT+START");

    i2sStream.begin();
    i2scopier.begin();
    
    vs1053.write(bt_wav_header, 44); 
    
}

void i2sreceiver_stop()
{
    LOGG_DEBUG("i2sreceiver_stop!");
    
    i2sreceiver_send("AT+END");
}

// ========== AT command stuff ==========

void i2sreceiver_serial_handle()
{
    if(serial_bt.available() > 0)
    {
        String str = serial_bt.readStringUntil('\n');      
    
        LOGG_DEBUG("Recv: " + str);
        i2sreceiver_command_parse(str);
    }
}

void i2sreceiver_send(String str)
{
    LOGG_DEBUG("Sending: " + str);
    serial_bt.print(str + '\n');
}

void i2sreceiver_command_parse(String command)
{
  if(command == "AT+AUDIOSTATE=PLAYING")
  {
    LOGG_INFO("Playing");
    information.audioPlayer.bluetoothMode = BT_PLAYING;
  }
  else if(command == "AT+AUDIOSTATE=PAUSED")
  {
    LOGG_INFO("Paused");
    information.audioPlayer.bluetoothMode = BT_PAUSED;
  }
  else if(command == "AT+AUDIOSTATE=STOPPED")
  {
    LOGG_INFO("Stopped");
    information.audioPlayer.bluetoothMode = BT_STOPPED;
  }
  else if(command.startsWith("AT+TITLE"))
  {
    information.audioPlayer.bluetoothTitle = command.substring(9);
    display_reset_scroll();
  }
  else if(command.startsWith("AT+ARTIST"))
  {
    information.audioPlayer.bluetoothArtist = command.substring(10);
    display_reset_scroll();
  }

  else if(command.startsWith("AT+CONNSTATE"))
  {
    information.audioPlayer.bluetoothConnectionStateStr = command.substring(13);
  }
  else if(command.startsWith("AT+RSSI"))
  {
    information.audioPlayer.bluetoothRSSI = command.substring(8).toInt();
  }
}