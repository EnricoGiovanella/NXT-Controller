#ifndef USBRS232_H
#define USBRS232_H

#include <stdint.h>
#include <stdio.h>

extern void * mainRs232(void *ptr);

extern void setRs232(unsigned char* device, uint32_t baud, uint8_t parity, uint8_t stop, uint8_t data);

extern void openRs232();

extern void closeRs232();

extern void transmitRs232(unsigned char* msg,size_t len);

#endif
