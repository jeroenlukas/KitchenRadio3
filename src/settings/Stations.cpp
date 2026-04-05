#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <YAMLDuino.h>

#include "Stations.h"

Station_t stations[CONF_WEBRADIO_STATIONS_MAX];

bool stations_load();

bool stations_load()
{
    JsonDocument docStations;

    Serial.println("Loading YAML stations");

    File file_stations = LittleFS.open("/settings/stations.yaml", "r");

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

    file_stations.close();

    // Convert yaml to json
    YAMLNode yaml_config = YAMLNode::loadString(file_content.c_str());

    String json_config;

    serializeYml(yaml_config.getDocument(), json_config, OUTPUT_JSON_PRETTY);

    auto error = deserializeJson(docStations, json_config);

    if(error) {
        Serial.printf("Unable to deserialize YAML to JsonObject: %s", error.c_str() );
        return false;
    }

    if(deserializeJson(docStations, json_config) != DeserializationError::Ok)
    {
        Serial.println("Error: deser error!");
        return false;
    }
    Serial.println("Deserialization ok");

    information.webRadio.station_count = docStations.size();
    Serial.println("Loaed " + String(information.webRadio.station_count) + " stations:");

    // Copy stations data to stations struct
    for(int i = 0; i < docStations.size(); i++)
    {
      stations[i].name = String(docStations[i]["name"]);
      stations[i].url = String(docStations[i]["url"]);
      Serial.println(" - "  + stations[i].name);
    }


    return true;
}