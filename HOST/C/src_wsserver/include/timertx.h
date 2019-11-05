#ifndef TIMERTX_H
#define TIMERTX_H


//initialize timer
extern void InitTimer();
//function to be inserted in the transmitter pthread
extern void runDelayTimer();

extern void setTXRqtCs(long millis);
extern void resetTXRqtCs();
extern int isTXRqtCs();

extern void setStartSerial(long millis);
extern void resetStartSerial();
extern int isStartSerialDelay();

extern void setSendStatus(long millis);
extern void resetSendStatus();
extern int isSendStatus();

//extern void startTimer();
//extern void stopTimer();

#endif
