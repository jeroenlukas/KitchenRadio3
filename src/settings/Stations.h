#ifndef KR_STATIONS_H
#define KR_STATIONS_H

#include <Arduino.h>

struct Station_t
{
  String name;
  String url;
};

extern Station_t stations[];

extern bool stations_load();

#endif