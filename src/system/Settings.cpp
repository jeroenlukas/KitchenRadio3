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

    Serial.println("json_config= " + json_config);

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

    Serial.println("json deviceName="  + String(docSettings["devicename"]));

    // Copy settings values to settings object
    settings.deviceName = String(docSettings["devicename"]);
    Serial.println("settigs.deviceName="  + settings.deviceName);
    settings.location = String(docSettings["location"]);
    settings.clock.timezone = String(docSettings["clock"]["timezone"]);
    settings.audio.tonecontrol.treble = docSettings["audio"]["tonecontrol"]["treble"];

    Serial.println("settigs.deviceName2="  + settings.deviceName);

    LOGG_INFO("Devicename: " + settings.deviceName);
    LOGG_INFO("Timezone: " + settings.clock.timezone);
    LOGG_INFO("Location: " + settings.location);
    LOGG_INFO("Treble: " + String(settings.audio.tonecontrol.treble));

    return true;
}