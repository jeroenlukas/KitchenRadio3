#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <YAMLDuino.h>

#include "Logger.h"

#include "Filemanager.h"
#include "Settings.h"

Settings_t settings;

bool settings_load();

bool settings_load()
{
    JsonDocument docSettings;

    LOGG_INFO("Loading YAML config");

    String file_content = filemgr_readfile("/settings/config.yaml");

    // Convert yaml to json
    YAMLNode yaml_config = YAMLNode::loadString(file_content.c_str());

    String json_config;

    serializeYml(yaml_config.getDocument(), json_config, OUTPUT_JSON_PRETTY);

    auto error = deserializeJson(docSettings, json_config);

    if(error) {
        LOGG_ERROR("Unable to deserialize YAML to JsonObject: " + String(error.c_str()) );
        return false;
    }

    if(deserializeJson(docSettings, json_config) != DeserializationError::Ok)
    {
        LOGG_ERROR("Error: deser error!");
        return false;
    }
    LOGG_INFO("Deserialization ok");

    // Copy settings values to settings object
    settings.deviceName = String(docSettings["devicename"]);
    settings.location = String(docSettings["location"]);
    settings.clock.timezone = String(docSettings["clock"]["timezone"]);
    settings.audio.tonecontrol.treble = docSettings["audio"]["tonecontrol"]["treble"];
    settings.audio.tonecontrol.treble_freq = docSettings["audio"]["tonecontrol"]["treble_freq"];
    settings.audio.tonecontrol.bass = docSettings["audio"]["tonecontrol"]["bass"];
    settings.audio.tonecontrol.bass_freq = docSettings["audio"]["tonecontrol"]["bass_freq"];
    settings.display.brightness_min = docSettings["display"]["brightness_min"];
    settings.display.brightness_max = docSettings["display"]["brightness_max"];

    LOGG_INFO("Devicename: " + settings.deviceName);
    LOGG_INFO("Timezone: " + settings.clock.timezone);
    LOGG_INFO("Location: " + settings.location);

    return true;
}