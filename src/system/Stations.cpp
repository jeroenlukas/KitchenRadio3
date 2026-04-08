#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <YAMLDuino.h>

#include "Logger.h"
#include "Stations.h"

#include "Filemanager.h"

Station_t stations[CONF_WEBRADIO_STATIONS_MAX];

bool stations_load();

bool stations_load()
{
    JsonDocument docStations;

    LOGG_INFO("Loading YAML stations");
    
    String file_content = filemgr_readfile("/settings/stations.yaml");

    // Convert yaml to json
    YAMLNode yaml_config = YAMLNode::loadString(file_content.c_str());

    String json_config;

    serializeYml(yaml_config.getDocument(), json_config, OUTPUT_JSON_PRETTY);

    auto error = deserializeJson(docStations, json_config);

    if(error) 
    {
        LOGG_ERROR("Unable to deserialize YAML to JsonObject: " + String(error.c_str()) );
        return false;
    }

    if(deserializeJson(docStations, json_config) != DeserializationError::Ok)
    {
        LOGG_ERROR("Error: deser error!");
        return false;
    }
    LOGG_INFO("Deserialization ok");

    information.webRadio.station_count = docStations.size();
    LOGG_INFO("Load " + String(information.webRadio.station_count) + " stations:");

    // Copy stations data to stations struct
    for(int i = 0; i < docStations.size(); i++)
    {
      stations[i].name = String(docStations[i]["name"]);
      stations[i].url = String(docStations[i]["url"]);
      LOGG_INFO(" - " + String(i) + ": " + stations[i].name);
    }


    return true;
}