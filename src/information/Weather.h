#ifndef KR_WEATHER_H
#define KR_WEATHER_H

//extern bool weather_retrieve();
extern bool weather_retrieve_40();
extern bool weather_geo(String location);
extern int weather_icon_to_glyph(String icon);
extern bool weather_forecast_1h();
extern bool weather_forecast_1d();


#endif