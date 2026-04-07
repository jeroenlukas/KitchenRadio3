#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <YAMLDuino.h>
#include <AdvancedLogger.h>

#include "Stations.h"

#include "Filemanager.h"

Station_t stations[CONF_WEBRADIO_STATIONS_MAX];

bool stations_load();

bool stations_load()
{
    JsonDocument docStations;

    LOG_INFO("Loading YAML stations");

    /*File file_stations = LittleFS.open("/settings/stations.yaml", "r");

    if(!file_stations)
    {
        Serial.print("Error: could not open config.yaml");
        return false;
    }

    String file_content;

    while(file_stations.available())
    {
        String data = file_stations.readString();
        file_content += data;
        Serial.print(data);
    }
    Serial.print("\n(end)\n");

    file_stations.close();*/
    
    String file_content = filemgr_readfile("/settings/stations.yaml");

    // Convert yaml to json
    YAMLNode yaml_config = YAMLNode::loadString(file_content.c_str());

    String json_config;

    serializeYml(yaml_config.getDocument(), json_config, OUTPUT_JSON_PRETTY);

    auto error = deserializeJson(docStations, json_config);

    if(error) 
    {
        LOG_ERROR("Unable to deserialize YAML to JsonObject: %s", error.c_str() );
        return false;
    }

    if(deserializeJson(docStations, json_config) != DeserializationError::Ok)
    {
        LOG_ERROR("Error: deser error!");
        return false;
    }
    LOG_INFO("Deserialization ok");

    information.webRadio.station_count = docStations.size();
    LOG_INFO("Load %d stations:", information.webRadio.station_count);

    // Copy stations data to stations struct
    for(int i = 0; i < docStations.size(); i++)
    {
      stations[i].name = String(docStations[i]["name"]);
      stations[i].url = String(docStations[i]["url"]);
      LOG_INFO(" - %02d: %s", i, stations[i].name);
    }


    return true;
}