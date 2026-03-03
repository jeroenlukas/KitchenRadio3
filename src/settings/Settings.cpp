#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <YAMLDuino.h>

DynamicJsonDocument settings(2048);

bool settings_load()
{
    Serial.println("Loading YAML config");

    File file_config = LittleFS.open("/settings/config.yaml", "r");

    if(!file_config)
    {
        Serial.print("Error: could not open config.yaml");
        return false;
    }

    String file_content;

    while(file_config.available())
    {
        String data = file_config.readString();
        file_content += data;
        Serial.print(data);
    }
    Serial.print("\n(end)\n");

    file_config.close();

    // Convert yaml to json
    YAMLNode yaml_config = YAMLNode::loadString(file_content.c_str());

    String json_config;

    serializeYml(yaml_config.getDocument(), json_config, OUTPUT_JSON_PRETTY);

    auto error = deserializeJson(settings, json_config);

    if(error) {
        Serial.printf("Unable to deserialize demo YAML to JsonObject: %s", error.c_str() );
        return false;
    }

    if(deserializeJson(settings, json_config) != DeserializationError::Ok)
    {
        Serial.println("Error: deser error!");
        return false;
    }
    Serial.println("Deserialization ok");
    return true;
}