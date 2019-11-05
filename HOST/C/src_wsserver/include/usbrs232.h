#ifndef USBRS232_H
#define USBRS232_H

#include <stdint.h>
#include <stdio.h>



#define MAX_DELAYSTART 2000	//millis
#define MINTIME_SENDSTATUS 5000	// 1 second = 1000 milliseconds
#define MIN_DELAYRQTCS 10000		//millis

#define MAX_REPLYCOUNT 3

extern void startSerialThread();

extern void stopSerialThread();

extern void openRs232();

extern void closeRs232();

extern void transmitRs232(uint8_t instant,uint8_t wsCod,uint8_t msgCod,uint8_t* msg,size_t len);

extern void setTXDataMsgOk(uint8_t val);

extern void setTXDataReplyMsg(uint8_t val);

extern uint8_t isTXDataMsgReadyBuf();

extern void resetFIFOSend();

extern uint8_t* setMsgTsystemCmd(uint8_t cmd);

#endif

