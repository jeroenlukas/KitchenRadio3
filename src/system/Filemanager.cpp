#include "../configuration/Config.h"
#include "../information/Information.h"

#include <Arduino.h>
#include <LittleFS.h>
#include <AdvancedLogger.h>

#include "Filemanager.h"

void filemgr_begin()
{
  LOG_INFO("Start LittleFS: %d", LittleFS.begin());
}

String filemgr_readfile(String path)
{
  File file = LittleFS.open(path, "r");

  if(!file)
  {
      LOG_ERROR("Error: could not open %s", path);
      return "";
  }

  String file_content;

  while(file.available())
  {
      String data = file.readString();
      file_content += data;
      LOG_INFO("%s", data);
  }
  LOG_INFO("--- (end) ---");

  file.close();

  return file_content;
}