#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "include/usbrs232.h"
#include "include/trasmitwsmsg.h"
#include "include/variables.h"

// fake serial connection
void serialReceive() {
	if(users > 0) {
		usleep( 1000000 );
	}
}

void * mainRs232(void *ptr) {
	fprintf(stderr,"thread seriale aperto\n");
	while(active) {
		serialReceive();
	};
	fprintf(stderr,"thread seriale chiuso\n");
}

void setRs232(unsigned char* device, uint32_t baud, uint8_t parity, uint8_t stop, uint8_t data) {
	fprintf(stderr, "device %s baud %u parity %u stop %u data %u ", device, baud,parity, stop, data);
}

void openRs232() {
	wsSerialStates.states = 1;
	wsSerialStates.codE = 0;
	transmitMsgWsSerialStates();
}

void closeRs232() {
	wsSerialStates.states = 0;
	wsSerialStates.codE = 0;
	transmitMsgWsSerialStates();
}


void transmitRs232(unsigned char* msg,size_t len) {
}
