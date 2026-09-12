#ifndef KR_DISPLAY_H
#define KR_DISPLAY_H

extern void display_begin();
extern void display_draw();
extern void display_draw_startup();

extern void display_draw_custominfo_system();
extern void display_draw_custominfo_smiley();
extern void display_draw_custominfo_weather();
extern void display_draw_systeminfo_overview();
extern void display_draw_systeminfo_advanced();
extern void display_draw_weather_forecast_hourly();
extern void display_draw_weather_forecast_daily();

extern void display_reset_scroll();
extern void display_update_scroll_offset();
extern void display_set_brightness_auto();

extern void display_popup(String message, int length = 3000);

extern void display_set_refresh_interval(uint32_t ms);

#endif