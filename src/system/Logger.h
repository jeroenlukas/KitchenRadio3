#ifndef KR_LOGGER_H
#define KR_LOGGER_H

extern void logger_begin();

extern void log_info(String message, String func);
extern void log_warning(String message, String func);
extern void log_debug(String message, String func);
extern void log_error(String message, String func);

extern void log_boot(String message);

extern int bootlog_cnt;
extern String bootlog[];

#define LOGG_INFO(message)  log_info(message, __func__)
#define LOGG_DEBUG(message)  log_debug(message, __func__)
#define LOGG_ERROR(message)  log_error(message, __func__)
#define LOGG_WARNING(message)  log_warning(message, __func__)

#endif