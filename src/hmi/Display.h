#ifndef KR_DISPLAY_H
#define KR_DISPLAY_H

extern void display_begin();
extern void display_draw();
extern void display_draw_startup();

extern void display_draw_custominfo_system();
extern void display_draw_custominfo_smiley();
extern void display_draw_custominfo_weather();

extern void display_reset_scroll();
extern void display_update_scroll_offset();

#endif