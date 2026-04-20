#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <LittleFS.h>
#include <SimpleCLI.h>

#include "../audio/Webradio.h"

#include "../audio/Audioplayer.h"
#include "../audio/I2SReceiver.h"
#include "../information/Weather.h"
#include "../information/Time.h"

#include "../system/Logger.h"
#include "../system/Filemanager.h"
#include "../system/Profiler.h"

#include "../hmi/Display.h"
#include "Frontpanel.h"
#include "Lamp.h"


SimpleCLI kr_cli;

Command cmd_reset;
Command cmd_help;

Command cmd_radio;

Command cmd_soundmode;
Command cmd_volume;
Command cmd_lamp;
Command cmd_bootlog;
Command cmd_log;
Command cmd_effect;
Command cmd_cat;
Command cmd_fake;
Command cmd_oled;
Command cmd_buzzer;
Command cmd_bt;
Command cmd_weather;
Command cmd_i2cping;
Command cmd_time;
Command cmd_treble;
Command cmd_bass;
Command cmd_mem;
Command cmd_profiler;

void cli_begin();
void cli_handle();


void cb_reset(cmd* c) 
{
    Command cmd(c);

    LOGG_INFO("I will be resetting!");

    delay(1000);

    ESP.restart();    
}

void cb_radio(cmd* c)
{
    Command cmd(c);
    
    if(cmd.getArg("start").isSet())
        webradio_connect(0);

    else if(cmd.getArg("stop").isSet())
        webradio_disconnect();

    else if(cmd.getArg("url").isSet())
        webradio_url_set(cmd.getArg("url").getValue());


}

void cb_profiler(cmd* c)
{
    profiler.print();
}

void cb_i2cping(cmd* c)
{
    frontpanel_i2c_ping();
}

void cb_treble(cmd* c)
{
    Command cmd(c);    

    if(cmd.getArgument(0).isSet())
    {
        uint8_t treble = cmd.getArgument(0).getValue().toInt();
        audioplayer_treble_set(treble);
    }
}

void cb_bass(cmd* c)
{
    Command cmd(c);    

    if(cmd.getArgument(0).isSet())
    {
        uint8_t bass = cmd.getArgument(0).getValue().toInt();
        audioplayer_bass_set(bass);
    }
}

void cb_soundmode(cmd* c)
{
    Command cmd(c);

    if(cmd.getArg("r").isSet()) audioplayer_mode_set(WEBRADIO);
    else if(cmd.getArg("o").isSet()) audioplayer_mode_set(OFF);
    else if(cmd.getArg("b").isSet()) audioplayer_mode_set(BLUETOOTH);
    
    else LOGG_ERROR("Error: invalid soundmode");   
}

void cb_volume(cmd* c)
{
    Command cmd(c);    

    if(cmd.getArgument(0).isSet())
    {
        uint8_t volume = cmd.getArgument(0).getValue().toInt();
        audioplayer_volume_set(volume);
    }
}

void cb_lamp(cmd* c)
{
    Command cmd(c);
    if(cmd.getArgument("h").isSet())
    {
        Serial.println("Setting hue");
        float h = cmd.getArgument("h").getValue().toFloat();
        lamp_sethue(h);
    }
    if(cmd.getArgument("s").isSet())
    {
        Serial.println("setting saturation");
        float s = cmd.getArgument("s").getValue().toFloat();
        lamp_setsaturation(s);
    }
    if(cmd.getArgument("l").isSet())
    {
        Serial.println("setting lightness");
        float l = cmd.getArgument("l").getValue().toFloat();
        lamp_setlightness(l);
    }
    if(cmd.getArgument("on").isSet())
    {
        lamp_on();
    }
    if(cmd.getArgument("off").isSet())
    {
        lamp_off();
    }

}

void cb_effect(cmd* c)
{
    Command cmd(c);

    if(cmd.getArgument("t").isSet())
    {
        //lamp_seteffecttype(cmd.getArgument("t").getValue().toInt());
    }
    if(cmd.getArgument("s").isSet())
    {
        lamp_seteffectspeed(cmd.getArgument("s").getValue().toFloat());
    }
}
/*
void cb_fake(cmd* c)
{
    Command cmd(c);
    if(cmd.getArgument("weatherstate").isSet())
    {
        int code= cmd.getArgument("weatherstate").getValue().toInt();
        log_debug("Faking weatherstate to " + String(code) );
        information.weather.stateCode = code;
    }
}

void cb_oled(cmd* c) 
{
    Command cmd(c);

    if(cmd.getArgument("brightness").isSet())
    {
        int val = cmd.getArgument("brightness").getValue().toInt();
        log_debug("[OLED] Set brightness to " + String(val) + "%");
        display_set_brightness(val);
    }

    else if(cmd.getArgument("contrast").isSet())
    {
        int val = cmd.getArgument("contrast").getValue().toInt();
        log_debug("[OLED] Set contast to " + String(val));
        u8g2.setContrast(val);
    }
    
    else if(cmd.getArgument("on").isSet())
    {
        log_debug("[OLED] Turn whole OLED on");
        u8g2.sendF("c", 0xAF);
    }    
    else if(cmd.getArgument("off").isSet())
    {
        log_debug("[OLED] Turn whole OLED off");
        u8g2.sendF("c", 0xAE);
    }

    else if(cmd.getArgument("allon").isSet())
    {
        log_debug("[OLED] Turn all pixels on");
        u8g2.sendF("c", 0xA5);
    }
    else if(cmd.getArgument("alloff").isSet())
    {
        log_debug("[OLED] Turn whole OLED off");
        u8g2.sendF("c", 0xA6);
    }
    else if(cmd.getArgument("normal").isSet())
    {
        log_debug("[OLED] Turn OLED normal");
        u8g2.sendF("c", 0xA6);
    }
    else if(cmd.getArgument("inverse").isSet())
    {
        log_debug("[OLED] Turn OLED inverse");
        u8g2.sendF("c", 0xA7);
    }
    
    if(cmd.getArgument("mcurr").isSet())
    {
        int val = constrain(cmd.getArgument("mcurr").getValue().toInt(), 0b0000, 0b1111);
        log_debug("[OLED] Set master current byte to " + String(val));
        u8g2.sendF("ca", 0xC7, val);
    }
    if(cmd.getArgument("pcv").isSet())
    {
        int val = constrain(cmd.getArgument("pcv").getValue().toInt(), 0b00000, 0b11111);
        log_debug("[OLED] Set pre charge voltage byte to " + String(val));
        u8g2.sendF("ca", 0xBB, val);
    }
    if(cmd.getArgument("clock").isSet())
    {
        int val = constrain(cmd.getArgument("clock").getValue().toInt(), 0b00000000, 0b11111111);
        log_debug("[OLED] Set clock/osc byte to " + String(val));
        u8g2.sendF("ca", 0xB3, val);
    }

}*/

void cb_buzzer(cmd* c)
{
    LOGG_DEBUG("Beep");
    //buzzer_beep(500);
    
    return;
}

void cb_bt(cmd* c)
{
    Command cmd(c);

    if(cmd.getArgument("reset").isSet())
    {
        i2sreceiver_send("AT+RESET");
    }
    if(cmd.getArgument("pause").isSet())
    {
        i2sreceiver_send("AT+PAUSE");
    }
    if(cmd.getArgument("play").isSet())
    {
        i2sreceiver_send("AT+PLAY");
    }
    if(cmd.getArgument("stop").isSet())
    {
        i2sreceiver_send("AT+STOP");
    }
    if(cmd.getArgument("start").isSet())
    {
        i2sreceiver_send("AT+START");
    }
    if(cmd.getArgument("end").isSet())
    {
        i2sreceiver_send("AT+END");
    }
    
    
    
}
/*
void cb_bootlog(cmd* c)
{
    Serial.print(bootlog);

    return;
}

void cb_log(cmd* c)
{
    Command cmd(c);
    String line = cmd.getArgument(0).getValue();
    log_debug(line);
}
*/
void cb_weather(cmd* c)
{
    weather_retrieve();
}

void cb_help(cmd* c)
{
    LOGG_INFO("--- Commands ---");
    LOGG_INFO(kr_cli.toString());
    LOGG_INFO("--- (end) ---");
}

void cb_time(cmd* c)
{
    time_waitForSync();
}

void cb_cat(cmd* c)
{
    Command cmd(c);
    String filename = cmd.getArgument(0).getValue();

    String content = filemgr_readfile(filename);

    LOGG_INFO("--- " + filename + " ---");
    LOGG_INFO("\n" + content);
    LOGG_INFO("--- (end) ---");
}

void cb_mem(cmd* c)
{
    Command cmd(c);

    int size_psram = ESP.getPsramSize();
    int used_psram = size_psram - ESP.getFreePsram();
    int pct_psram = ((double)used_psram / (double)size_psram) * 100;

    int size_heap = ESP.getHeapSize();
    int used_heap = size_heap - ESP.getFreeHeap();
    int pct_heap = ((double)used_heap / (double)size_heap) * 100;
    
    int size_sketch = ESP.getFreeSketchSpace() + ESP.getSketchSize(); // Max sketch size!
    int used_sketch = ESP.getSketchSize();
    int pct_sketch = ((double)used_sketch / (double)size_sketch) * 100;
    
     

    LOGG_INFO("--- MEMORY ---");
    LOGG_INFO("Sketch: " + String(used_sketch) + " B used out of " + String(size_sketch) + " B (" + String(pct_sketch) + "%)");
    LOGG_INFO("Heap: " + String(used_heap) + " B used out of " + String(size_heap) + " B (" + String(pct_heap) + "%)");
    LOGG_INFO("PSRAM: " + String(used_psram) + " B used out of " + String(size_psram) + " B (" + String(pct_psram) + "%)");
    
}

void cb_error(cmd_error* e) {
    CommandError cmdError(e); // Create wrapper object

    LOGG_ERROR("CLI: " + cmdError.toString());

    if (cmdError.hasCommand()) {
        Serial.print("Did you mean \"");
        LOGG_INFO("Did you mean: " + cmdError.getCommand().toString() + " ?");
    }
}

void cli_begin(void)
{
    kr_cli.setOnError(cb_error);

    // > reset
    cmd_reset = kr_cli.addCmd("reset", cb_reset);
    cmd_reset.setDescription("- Reset the ESP32");

    // > help
    cmd_help = kr_cli.addCmd("help", cb_help);
    cmd_help.setDescription("- Show this help");

    // > radio
    cmd_radio = kr_cli.addCmd("radio", cb_radio);
    cmd_radio.addFlagArg("start");
    cmd_radio.addFlagArg("stop");
    cmd_radio.addArgument("url", "");
    cmd_log.setDescription("- Control the web radio");

    // > volume
    cmd_volume = kr_cli.addSingleArgCmd("volume", cb_volume);
    cmd_volume.setDescription("- Set the volume (0..100)");

    // > cat
    cmd_cat = kr_cli.addSingleArgumentCommand("cat", cb_cat);
    cmd_cat.setDescription("- Print contents of a file. Example: cat /settings/stations.json");


    // > soundmode
    cmd_soundmode = kr_cli.addCommand("soundmode", cb_soundmode);
    cmd_soundmode.addFlagArgument("o/ff");
    cmd_soundmode.addFlagArgument("r/adio");
    cmd_soundmode.addFlagArgument("b/luetooth");    
    cmd_soundmode.setDescription("- Set the soundmode");

    // > weather
    cmd_weather = kr_cli.addCommand("weather", cb_weather);
    cmd_weather.setDescription("- Update weather info");

    // > i2cping
    cmd_i2cping = kr_cli.addCommand("i2cping", cb_i2cping);
    cmd_i2cping.setDescription("- Scan the I2C bus for devices");

    // > treble
    cmd_treble = kr_cli.addSingleArgCmd("treble", cb_treble);
    cmd_treble.setDescription("- Set the treble (0..100)");

    // > bass

    
    // > mem
    cmd_mem = kr_cli.addCommand("mem", cb_mem);
    cmd_mem.setDescription("- Show memory statistics");

    // > profiler
    cmd_profiler = kr_cli.addCommand("profiler", cb_profiler);
    cmd_profiler.setDescription("- Show time profiler results");
    
/*
    // > bootlog
    cmd_bootlog = kr_cli.addCommand("bootlog", cb_bootlog);
    cmd_bootlog.setDescription("- Print the boot log with timestamps");

    // > help
*/

    // > lamp
    cmd_lamp = kr_cli.addCmd("lamp", cb_lamp);
    cmd_lamp.addArgument("h/ue", "0.0");
    cmd_lamp.addArgument("s/aturation", "0.0");
    cmd_lamp.addArgument("l/ightness", "0.0");    
    cmd_lamp.addFlagArgument("on");
    cmd_lamp.addFlagArgument("off");
    cmd_lamp.setDescription("- Set the lamp hue (0.0 - 1.0), saturation (0.0 - 1.0) and/or lightness (0.0 - 0.5)");
/*
    // > effect
    cmd_effect = kr_cli.addCmd("effect", cb_effect);
    cmd_effect.addArgument("t/ype", "0");
    cmd_effect.addArgument("s/peed", "0");
    cmd_effect.setDescription("- Set lamp effect and parameters");
    
    // > log
    cmd_log = kr_cli.addSingleArgCmd("log", cb_log);
    cmd_log.setDescription("- Print a debug message");


    // > fake
    cmd_fake = kr_cli.addCmd("fake", cb_fake);
    cmd_fake.addArgument("weatherstate", "804");

    // > oled
    cmd_oled = kr_cli.addCmd("oled", cb_oled);
    cmd_oled.addArgument("b/rightness", "100");
    cmd_oled.addArgument("c/ontrast", "100");
    cmd_oled.addFlagArgument("on");
    cmd_oled.addFlagArgument("off");
    cmd_oled.addFlagArgument("allon");
    cmd_oled.addFlagArgument("alloff");
    cmd_oled.addFlagArgument("normal");
    cmd_oled.addFlagArgument("inverse");
    cmd_oled.addArgument("clock", "0");
    cmd_oled.addArgument("pcv", "0");    // Pre charge voltage
    cmd_oled.addArgument("mcurr", "0");  // Master current

    
*/

    // > buzzer
    cmd_buzzer = kr_cli.addSingleArgCmd("buzzer", cb_buzzer);

    // > bt
    cmd_bt = kr_cli.addCmd("bt", cb_bt);
    cmd_bt.setDescription("- Control bluetooth module");
    cmd_bt.addFlagArgument("reset");
    cmd_bt.addFlagArgument("pause");
    cmd_bt.addFlagArgument("play");
    cmd_bt.addFlagArgument("stop");
    cmd_bt.addFlagArgument("start");
    cmd_bt.addFlagArgument("end");

    // > time
    cmd_time = kr_cli.addCmd("time", cb_time);
    cmd_time.setDescription("- Update time from the internet");


}

void cli_parse(String input)
{
    LOGG_DEBUG(" >> " + input);
    kr_cli.parse(input);
}

void cli_handle()
{
    if (Serial.available()) 
    {
        static char commandbuffer[100] = "";
        static uint8_t commandbuffer_idx = 0;

        char inp = Serial.read();
        Serial.print('\r');
        //Serial.print((char)inp);
        
        commandbuffer[commandbuffer_idx] = inp;
        commandbuffer_idx ++;
        commandbuffer[commandbuffer_idx] = '\0';

        //Serial.print(commandbuffer);

        if(inp == '\n')
        {        
            //Serial.print("**n");
            String commandstr = String(commandbuffer);
            cli_parse(commandstr);
            commandbuffer_idx = 0;
        }
    }
}