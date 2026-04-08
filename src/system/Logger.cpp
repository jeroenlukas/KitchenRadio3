#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
//#include <AudioTools.h>
#include <AudioLogger.h>
#include <AdvancedLogger.h>

#include "Logger.h"

void logger_begin()
{
    AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);
    AdvancedLogger::begin();

    LOGG_INFO("INFO log!!!");
    LOGG_ERROR("Error log!");
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