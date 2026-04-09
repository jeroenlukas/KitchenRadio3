#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <LittleFS.h>

#include "Logger.h"

#include "Filemanager.h"

void filemgr_begin()
{
  LOGG_INFO("Start LittleFS: " + String(LittleFS.begin()));
}

String filemgr_readfile(String path)
{
  File file = LittleFS.open(path, "r");

  if(!file)
  {
      LOGG_ERROR("Error: could not open " + path);
      return "";
  }

  String file_content;

  while(file.available())
  {
      String data = file.readString();
      file_content += data;
      //LOGG_INFO(data);
  }
  //LOGG_INFO("--- (end) ---");

  file.close();

  return file_content;
}