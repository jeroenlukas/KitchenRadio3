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

    LOG_INFO("INFO log!!!");
    LOG_ERROR("Error log!");
}