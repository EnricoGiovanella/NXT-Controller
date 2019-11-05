#include <time.h> 
#include <sys/time.h>   
#include <sys/resource.h> 
#include <stdio.h>

#include "include/timertx.h"


// one shot timer 1 for start of communication 1 for reception timeout 1 for transmission timeout
struct {
	struct timespec reqNsleep;
	struct timespec remNsleep;

	struct timespec finish;

	struct timespec startTXRqtCs;
	int enableTXRqtCs;
	long dmillisTXRqtCs;
	int flagTXRqtCs;

	struct timespec startSerial;
	int enableSerial;
	long dmillisSerial;
	int flagSerial;	

	struct timespec startSendStatus;
	int enableSendStatus;
	long dmillisSendStatus;
	int flagSendStatus;	
} timerData;


void InitTimer() {
	//fprintf(stderr,"\nInitTimer()\n");
	timerData.reqNsleep.tv_sec = 0;
        timerData.reqNsleep.tv_nsec = 500000;	// 0.5 milliseconds

	timerData.enableTXRqtCs = 0;
	timerData.dmillisTXRqtCs = 0;
	timerData.flagTXRqtCs = 0;

	timerData.enableSerial = 0;
	timerData.dmillisSerial = 0;
	timerData.flagSerial = 0;

	timerData.enableSendStatus = 0;
	timerData.dmillisSendStatus = 0;
	timerData.flagSendStatus = 0;
}


// request message status

void setTXRqtCs(long millis) {
	timerData.enableTXRqtCs = 1;
	timerData.dmillisTXRqtCs = millis * 10;
	clock_gettime(CLOCK_REALTIME, &timerData.startTXRqtCs);
}

void resetTXRqtCs() {
	timerData.enableTXRqtCs = 0;
	timerData.dmillisTXRqtCs = 0;
	timerData.flagTXRqtCs = 0;
}

int isTXRqtCs() {
	return timerData.flagTXRqtCs;
}

// connection start delay
void setStartSerial(long millis) {
	timerData.enableSerial = 1;
	timerData.dmillisSerial = millis * 10;
	clock_gettime(CLOCK_REALTIME, &timerData.startSerial);
}

void resetStartSerial() {
	timerData.enableSerial = 0;
	timerData.dmillisSerial = 0;
	timerData.flagSerial = 0;
}

int isStartSerialDelay() {
	return timerData.flagSerial;
}

// Send message status to interface
void setSendStatus(long millis) {
	timerData.enableSendStatus = 1;
	timerData.dmillisSendStatus = millis * 10;
	clock_gettime(CLOCK_REALTIME, &timerData.startSendStatus);
}

void resetSendStatus() {
	timerData.enableSendStatus = 0;
	timerData.dmillisSendStatus = 0;
	timerData.flagSendStatus = 0;
}

int isSendStatus() {
	return timerData.flagSendStatus;
}

void calculateTimer() {
	long dmillis;
	clock_gettime(CLOCK_REALTIME, &timerData.finish);

	if(timerData.enableTXRqtCs) {
		dmillis = (timerData.finish.tv_sec - timerData.startTXRqtCs.tv_sec) *10000;
		dmillis += (timerData.finish.tv_nsec - timerData.startTXRqtCs.tv_nsec)/100000;
		if(dmillis > timerData.dmillisTXRqtCs) timerData.flagTXRqtCs = 1;
	}

	if(timerData.enableSerial) {
		dmillis = (timerData.finish.tv_sec -timerData.startSerial.tv_sec) * 10000;
		dmillis += (timerData.finish.tv_nsec - timerData.startSerial.tv_nsec) /100000;
		if(dmillis > timerData.dmillisSerial) timerData.flagSerial = 1;
	}

	if(timerData.enableSendStatus) {
		dmillis = (timerData.finish.tv_sec -timerData.startSendStatus.tv_sec) * 10000;
		dmillis += (timerData.finish.tv_nsec - timerData.startSendStatus.tv_nsec) /100000;
		if(dmillis > timerData.dmillisSendStatus) timerData.flagSendStatus = 1;
	}
}

//function to be inserted in the transmitter pthread
void runDelayTimer() {
	nanosleep(&timerData.reqNsleep , &timerData.remNsleep);
	calculateTimer();
}


