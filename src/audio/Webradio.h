#ifndef KR_WEBRADIO_H
#define KR_WEBRADIO_H

extern void webradio_init();
extern void webradio_connect(int station_idx);
extern void webradio_handle();
extern void webradio_disconnect();
extern void webradio_url_set(String urlNew);

#endif