#ifndef ACTIONMSGAVR_H
#define ACTIONMSGAVR_H

#include <stdint.h>
#include <stdio.h>





extern void actionSystemOk(uint8_t* buf,size_t len);
extern void actionSystemError(uint8_t* buf,size_t len);
extern int isMoveCmdNotPresent();
extern void actionCommandStatus(uint8_t* buf,size_t len);
extern void actionDefaultData(uint8_t* buf,size_t len);

extern void initTestCommandStatus();
extern void dispatchCommandStatus();

#endif
