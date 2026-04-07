#ifndef KR_CONFIG_H
#define KR_CONFIG_H

#include "ConfigPinout.h"
#include "ConfigSecrets.h"

// AudioTools configuration
#define DEFAULT_BUFFER_SIZE 10*4096//(1024*100)
#define VS1053_EXT 1

// AdvancedLogger configuration
#define ADVANCED_LOGGER_DISABLE_FILE_LOGGING 1

// Webradio
#define CONF_WEBRADIO_MIN_BYTES    (256 * 1024)   // How many bytes needed in buffer before we start playing it
#define CONF_WEBRADIO_MIN_BYTES_HALT  (10*1024)   // Low treshold, at this point we stop playing (and reconnect?)
#define CONF_WEBRADIO_BYTESTOGET (512)            // How much bytes do we copy from the stream each time. Keeping this value low seems to increase reliability?
#define CONF_WEBRADIO_BUFFERSIZE (2*1024 * 1024)  // Size of the circular buffer for radio streams
#define CONF_WEBRADIO_STATIONS_MAX  50

#endif