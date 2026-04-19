#ifndef KR_I2SRECEIVER_H
#define KR_I2SRECEIVER_H

extern void i2sreceiver_init();
extern void i2sreceiver_handle();

extern void i2sreceiver_start();
extern void i2sreceiver_stop();
extern void i2sreceiver_serial_handle();
extern void i2sreceiver_send(String str);
extern void i2sreceiver_playpause();


#endif