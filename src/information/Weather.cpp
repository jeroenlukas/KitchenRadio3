#include <WiFi.h>
#include <HTTPClient.h>
#include <ezTime.h>
#include <ArduinoJson.h>
#include "Information.h"
#include "Time.h"
#include "../configuration/Config.h"
#include "../system/Settings.h"
#include "../system/Logger.h"
#include "../information/Weather.h"
#include "../hmi/Display.h"

int weather_statecode_to_glyph(int statecode);
int weather_icon_to_glyph(String icon);
int weather_windkmh_to_beaufort(double wind_kmh);

HTTPClient http;

const String key = CONFIG_SECRETS_OPENWEATHER_KEY;

float weather_temperature = 0.0;
float windspeed_kmh = 0.0;
float windspeed = 0.0;

int weather_temperature_int = 0;

// Get lat/lon coordinates from a location string
bool weather_geo(String location)
{
    LOGG_INFO("Retrieving geo info (Geocoding API 1.0)");
    String endpoint = "http://api.openweathermap.org/geo/1.0/direct?q=" + location +"&appid=";
    LOGG_DEBUG("Endpoint: " + endpoint);
    bool ret = false;
    http.begin(endpoint + key);

    int httpCode = http.GET();

    if (httpCode > 0)
    {
        String payload = http.getString();
        JsonDocument doc;

        deserializeJson(doc, payload);
        Serial.print(payload);

        information.weather.lat = (double)doc[0]["lat"];
        information.weather.lon = (double)doc[0]["lon"];

        LOGG_DEBUG("Lon: " + String(information.weather.lon, 5));
        LOGG_DEBUG("Lat: " + String(information.weather.lat, 5));


        return true;
    }

    return false;
}

// Convert wind direction degrees to string
String weather_wind_direction_convert(int degrees) {
    // Normalize to 0..359
    degrees %= 360;
    if (degrees < 0)
        degrees += 360;

    // 16 compass sectors, each 22.5 degrees
    int index = (degrees + 11) / 22;

    switch (index) {
        case 0:
        case 16: return "N";
        case 1:   return "NNE";
        case 2:   return "NE";
        case 3:   return "ENE";
        case 4:   return "E";
        case 5:   return "ESE";
        case 6:   return "SE";
        case 7:   return "SSE";
        case 8:   return "S";
        case 9:   return "SSW";
        case 10:  return "SW";
        case 11:  return "WSW";
        case 12:  return "W";
        case 13:  return "WNW";
        case 14:  return "NW";
        case 15:  return "NNW";
    }

    return "?";
}

bool weather_retrieve_40()
{
    LOGG_INFO("Retrieving weather info (One Call 4.0)");

    if((information.weather.lon == 0) && (information.weather.lat == 0))
    {
        // Get geo coords first
        if(!weather_geo(settings.location))
            return false;
    }

    String endpoint = "http://api.openweathermap.org/data/4.0/onecall/current?lat=" + String(information.weather.lat, 5) + "&lon=" + String(information.weather.lon, 5) +"&units=metric&lang=nl&APPID=";
    LOGG_DEBUG("Endpoint: " + endpoint);
    bool ret = false;
    http.begin(endpoint + key);

    int httpCode = http.GET();

    if (httpCode > 0)
    {
        String payload = http.getString();
        JsonDocument doc;

        deserializeJson(doc, payload);
        LOGG_DEBUG(payload);

        String weather_type = doc["data"][0]["weather"][0]["description"];        
        information.weather.stateShort = weather_type;

        information.weather.temperature = doc["data"][0]["temp"];        

        information.weather.windSpeedKmh = ((double)(doc["data"][0]["wind_speed"])) * 3.6;
        information.weather.windSpeedBft = weather_windkmh_to_beaufort(information.weather.windSpeedKmh);
        information.weather.wind_direction_deg = round(((float)(doc["data"][0]["wind_deg"])+11) / 22.5) * 22.5;
        information.weather.wind_direction_str = weather_wind_direction_convert(information.weather.wind_direction_deg);

        information.weather.stateCode = (int)(doc["data"][0]["weather"][0]["id"]);
        information.weather.temperature_feelslike = doc["data"][0]["feels_like"];
        information.weather.pressure = doc["data"][0]["pressure"];
        information.weather.humidity = doc["data"][0]["humidity"];
        
        information.weather.sunrise = doc["data"][0]["sunrise"];
        information.weather.sunset = doc["data"][0]["sunset"];                
        information.weather.sunrise_str = tzLocal.dateTime(information.weather.sunrise, ezLocalOrUTC_t::UTC_TIME, "H:i");        
        information.weather.sunset_str = tzLocal.dateTime(information.weather.sunset, ezLocalOrUTC_t::UTC_TIME, "H:i");


        String weather_icon = doc["data"][0]["weather"][0]["icon"];

        LOGG_DEBUG("Rounded wind direction: " + String(information.weather.wind_direction_deg) );
        information.weather.icon = weather_icon;

        display_popup("Weather info retrieved");

        return true;
    }

    return false;
}

bool weather_forecast_1h()
{
    LOGG_INFO("Retrieving weather forecast 1h (One Call 4.0)");

    if((information.weather.lon == 0) && (information.weather.lat == 0))
    {
        // Get geo coords first
        if(!weather_geo(settings.location))
            return false;
    }

    String endpoint = "http://api.openweathermap.org/data/4.0/onecall/timeline/1h?lat=" + String(information.weather.lat, 5) + "&lon=" + String(information.weather.lon, 5) +"&units=metric&lang=nl&APPID=";
    LOGG_DEBUG("Endpoint: " + endpoint);
    bool ret = false;
    http.begin(endpoint + key);

    int httpCode = http.GET();

    if (httpCode > 0)
    {
        String payload = http.getString();
        JsonDocument doc;

        deserializeJson(doc, payload);
        LOGG_DEBUG(payload);

        int hours = 7;
        for(int i = 0; i < hours; i++)
        {
            information.weather.forecast_1h_temp[i] = doc["data"][i+1]["temp"];
            information.weather.forecast_1h_hour[i] = tzLocal.hour(doc["data"][i+1]["dt"], UTC_TIME);
            String weather_desc = doc["data"][i+1]["weather"][0]["description"];
            information.weather.forecast_1h_description[i] = weather_desc;
            information.weather.forecast_1h_windspeed_bft[i] = weather_windkmh_to_beaufort((double)doc["data"][i+1]["wind_speed"]*3.6);
            information.weather.forecast_1h_winddir[i] = round(((float)(doc["data"][i+1]["wind_deg"])+11) / 22.5) * 22.5;   // round(((float)(doc["data"][0]["wind_deg"])+11) / 22.5) * 22.5;
            LOGG_DEBUG(String("Temperature at " + String( information.weather.forecast_1h_hour[i]) + ":00 hour: " + String(information.weather.forecast_1h_temp[i])));
        }

        return true;
    }

    return false;
}

// Get daily forecast
// Daily forecast will also contain moon info (rise/set/phase)
bool weather_forecast_1d()
{
    LOGG_INFO("Retrieving weather forecast 1d (One Call 4.0)");

    if((information.weather.lon == 0) && (information.weather.lat == 0))
    {
        // Get geo coords first
        if(!weather_geo(settings.location))
            return false;
    }

    String endpoint = "http://api.openweathermap.org/data/4.0/onecall/timeline/1day?lat=" + String(information.weather.lat, 5) + "&lon=" + String(information.weather.lon, 5) +"&units=metric&lang=nl&APPID=";
    LOGG_DEBUG("Endpoint: " + endpoint);
    bool ret = false;
    http.begin(endpoint + key);

    int httpCode = http.GET();

    if (httpCode > 0)
    {
        String payload = http.getString();
        JsonDocument doc;

        deserializeJson(doc, payload);
        LOGG_DEBUG(payload);

        int days = 4;
        for(int i = 0; i < days; i++)
        {
            information.weather.forecast_1d_temp[i] = doc["data"][i]["temp"]["max"];
            information.weather.forecast_1d_day[i] = tzLocal.dateTime(doc["data"][i]["dt"], UTC_TIME, "D"); //tzLocal.day(doc["data"][i]["dt"], UTC_TIME);
            String weather_desc = doc["data"][i]["weather"][0]["description"];
            information.weather.forecast_1d_description[i] = weather_desc;
            information.weather.forecast_1d_windspeed_bft[i] = weather_windkmh_to_beaufort((double)doc["data"][i]["wind_speed"]*3.6);
            information.weather.forecast_1d_winddir[i] = round(((float)(doc["data"][i+1]["wind_deg"])+11) / 22.5) * 22.5;

            //LOGG_DEBUG(String("Temperature at day " + String( information.weather.forecast_1d_day[i]) + ": " + String(information.weather.forecast_1d_temp[i])));
        }

        return true;
    }

    return false;
}

bool weather_retrieve()
{
    LOGG_ERROR("weather_retrieve is deprecated!");
    return false;

    LOGG_INFO("Retrieving weather info");
    String endpoint = "http://api.openweathermap.org/data/2.5/weather?q=" + settings.location +"&units=metric&lang=nl&APPID=";
    LOGG_DEBUG("Endpoint: " + endpoint);
    bool ret = false;
    http.begin(endpoint + key);

    int httpCode = http.GET();

    if (httpCode > 0)
    {
        String payload = http.getString();
        JsonDocument doc;

        deserializeJson(doc, payload);
        Serial.print(payload);

        String weather_type = doc["weather"][0]["description"];        
        information.weather.stateShort = weather_type;
        information.weather.temperature = doc["main"]["temp"];        
        information.weather.windSpeedKmh = ((double)(doc["wind"]["speed"])) * 3.6;
        information.weather.windSpeedBft = weather_windkmh_to_beaufort(information.weather.windSpeedKmh);
        information.weather.stateCode = (int)(doc["weather"][0]["id"]);
        information.weather.temperature_feelslike = doc["main"]["feels_like"];
        information.weather.pressure = doc["main"]["pressure"];
        information.weather.humidity = doc["main"]["humidity"];
        
        information.weather.sunrise = doc["sys"]["sunrise"];
        information.weather.sunset = doc["sys"]["sunset"];                
        information.weather.sunrise_str = tzLocal.dateTime(information.weather.sunrise, ezLocalOrUTC_t::UTC_TIME, "H:i");        
        information.weather.sunset_str = tzLocal.dateTime(information.weather.sunset, ezLocalOrUTC_t::UTC_TIME, "H:i");


        String weather_icon = doc["weather"][0]["icon"];
        information.weather.icon = weather_icon;
        

        Serial.println("Weather icon:" + information.weather.icon);
        
        
        ret = true;
    }
    else
        Serial.println("weather_retrieve: Error on HTTP request");

    http.end();
    
    display_popup("Updated weather info");

    return ret;
}

int weather_statecode_to_glyph(int statecode)
{
    //log_debug("Statecode : " + String(statecode));

    int glyph = 0;

    switch(statecode)
    {
        case 200:
        case 201:
        case 202:
        case 210:
        case 211:
        case 212:
        case 221:
        case 230:
        case 231:
        case 232:
            glyph = 70;   // Thunderstorm
            break;
        case 300:
        case 301:
        case 302:
        case 310:
        case 311:
        case 312:
        case 313:
        case 314:
        case 321:
            glyph = 39;   // Drizzle
            break;
        case 500:
        case 501:
        case 502:
        case 503:
        case 504:
        case 511:
        case 520:
        case 521:
        case 522:
        case 531:
            glyph = 36;  // Rain
            break;
        case 600:
        case 601:
        case 602:
        case 611:
        case 612:
        case 613:
        case 615:
        case 616:
        case 620:
        case 621:
        case 622:
            glyph = 57; // Snow
            break;
        case 701:
        case 711:
        case 741:
            glyph = 63; // Mist
            break;
        case 800:
            glyph = 73; // Clear
            break;
        case 801:
        case 802:
        case 803:
        case 804:
            glyph = 33; // Clouds
            break;
        default:
            glyph = 96; // Unknown
            break;

    }

    //log_debug("Glyph: " + String(glyph));

    return glyph;

    
}

int weather_icon_to_glyph(String icon)
{
    if(icon == "01d") return 73; // Clear
    if(icon == "01n") return 78;

    if(icon == "02d") return 34; // Few clouds
    if(icon == "02n") return 35;

    if(icon == "03d") return 33; // Scattered clouds
    if(icon == "03n") return 33;

    if(icon == "04d") return 33; // Broken clouds
    if(icon == "04n") return 33; 

    if(icon == "09d") return 36; // Shower rain (heavier than 'rain')
    if(icon == "09n") return 36;

    if(icon == "10d") return 37; // Rain
    if(icon == "10n") return 38;

    if(icon == "11d") return 70; // Thunderstorm
    if(icon == "11n") return 70;

    if(icon == "13d") return 54; // Snow
    if(icon == "13n") return 56;

    if(icon == "50d") return 63; // Mist
    if(icon == "50n") return 65;

    return 0;
}

int weather_windkmh_to_beaufort(double wind_kmh)
{
    if(wind_kmh < 1.0) return 0;
    if(wind_kmh <= 5.0) return 1;
    if(wind_kmh <= 11.0) return 2;
    if(wind_kmh <= 19.0) return 3;
    if(wind_kmh <= 28.0) return 4;
    if(wind_kmh <= 38.0) return 5;
    if(wind_kmh <= 49.0) return 6;
    if(wind_kmh <= 61.0) return 7;
    if(wind_kmh <= 74.0) return 8;
    if(wind_kmh <= 88.0) return 9;
    if(wind_kmh <= 102.0) return 10;
    if(wind_kmh <= 117.0) return 11;
    if(wind_kmh > 118.0) return 12;

    return 0;
}