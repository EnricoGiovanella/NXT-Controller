#include <stdio.h>
#include <stdint.h>
#include "include/messagews.h"
#include "include/receivewsmsg.h"
#include "include/transmitwsmsg.h"
#include "include/actionwsmsg.h"
#include "include/tools.h"
#include "include/wsserver.h"
#include "include/variables.h"

extern inline uint32_t convertMsg2U32(unsigned char* in,uint8_t convEndians);

WS_serialOpen wsSerialOpen;
WS_serialStates wsSerialStates;

//----------------------------- receive message WS SERIALOPEN -----------------------------

void receiveMsgWsSerialOpen(unsigned char* msg,size_t len) {
	for(uint8_t i; i<20; i++) wsSerialOpen.device[i] = msg[0 + i];
	wsSerialOpen.baud = convertMsg2U32(&msg[20],convWsEndians);
	wsSerialOpen.parity = (uint8_t) msg[24];
	wsSerialOpen.stop = (uint8_t) msg[25];
	wsSerialOpen.data = (uint8_t) msg[26];
	wsActionSerialOpen();
}

//---------------------------- receive message WS SERIALCLOSE -----------------------------

void receiveMsgWsSerialClose(unsigned char* msg,size_t len) {
	wsActionSerialClose();
}

//-------------------------- transmit WS SERIALSTATES message -----------------------------

void transmitMsgWsSerialStates() {
	unsigned char msg[sizeof(WS_serialStates) + 1];

	msg[0] = (unsigned char) wsSerialStates.states;
	msg[1] = (unsigned char) wsSerialStates.codE;
	sendBinaryAllUsers(msg,sizeof(WS_serialStates) + 1);
}

//--------- end of message transmission reception specific functions ----------------------
//-----------------------------------------------------------------------------------------
// called from wsserver.c onMessage (websocket)
// decode the message in the appropriate structure by calling the function receive message
void receiveBinaryWsMsg(unsigned char* msg,size_t len) {
	uint8_t codMsg = (uint8_t) msg[0];
	switch(codMsg) {
		case WSMSG_SERIALOPEN:
			receiveMsgWsSerialOpen(&msg[1],len);
		return;
		case WSMSG_SERIALCLOSE:	// no data
			receiveMsgWsSerialClose(&msg[1],len);
			//wsActionSerialClose();
		return;
	}
}
