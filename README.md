# KitchenRadio 3.0

The new KitchenRadio project, using [Arduino Audio Tools](https://github.com/pschatzmann/arduino-audio-tools).
Moved from PlatformIO to Arduino IDE.

Designed for the ESP32 S3 WROOM with 8 MB PSRAM. See esp32settings.png for settings.

Uses the external VS1053 lib from https://github.com/baldram/ESP_VS1053_Library/tree/master. Please add the softReset() function to VS1053Stream.h !