#ifndef RECEIVEWSMSG_H
#define RECEIVEWSMSG_H

#include <stdio.h>

//----------------------------- receive message WS SERIALOPEN -----------------------------

extern void receiveMsgWsSerialOpen(unsigned char* msg,size_t len);

//----------------------------- receive message WS SERIALCLOSE ----------------------------

extern void receiveMsgWsSerialClose(unsigned char* msg,size_t len);

//--------- end of message transmission reception specific functions ----------------------
//-------------------------------- main recieve function ----------------------------------

extern void receiveBinaryWsMsg(unsigned char* msg,size_t len);

#endif
