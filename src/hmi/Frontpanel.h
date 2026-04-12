#ifndef KR_FRONTPANEL_H
#define KR_FRONTPANEL_H

extern void frontpanel_begin();
extern void frontpanel_handle();
extern void frontpanel_i2c_ping();
extern void frontpanel_buttons_read();
extern void frontpanel_ldr_read();

#endif