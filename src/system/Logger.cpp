#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <AudioLogger.h>
#include <AdvancedLogger.h>

#include "..\hmi\Display.h"

#include "Logger.h"

int bootlog_cnt = 0;
String bootlog[30];

void logger_begin()
{
    AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);   
    
}

void log_info(String message, String func)
{
    Serial.println("[INFO ] [" + func + "] " + message);
}

void log_debug(String message, String func)
{
    Serial.println("[DEBUG] [" + func + "] " + message);
}

void log_error(String message, String func)
{
    Serial.println("[ERROR] [" + func + "] " + message);
}

void log_warning(String message, String func)
{
    Serial.println("[WARN ] [" + func + "] " + message);
}

// For startup
void log_boot(String message)
{
    bootlog[bootlog_cnt++] = message;

    display_draw_startup();
    Serial.println("[BOOT ] " + message);
}