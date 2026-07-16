#ifndef KR_FILEMANAGER_H
#define KR_FILEMANAGER_H

#include <Arduino.h>

extern void filemgr_begin();
extern String filemgr_readfile(String path);
extern bool filemgr_writefile(String path, String content);

#endif