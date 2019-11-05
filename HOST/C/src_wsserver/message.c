#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "include/message.h"
//#include "include/receivewsmsg.h"
//#include "include/trasmitwsmsg.h"
#include "include/actionwsmsg.h"
//#include "include/tools.h"
#include "include/wsserver.h"
//#include "include/timertx.h"



// ----------------------- instances of internal structures ---------------------------------
 
//WS_commandStatus workCommandStatus;
WS_paramAvr workParamAvr;

// ----------------------- instances of web socket message structures -----------------------

WS_serialOpen wsSerialOpen;
WS_serialStates wsSerialStates;

WS_commandStatus wsCommandStatus;
WS_paramAvr wsParamAvr;
WS_move wsMove;
WS_paramAvr wsParam2avr;

WS_directMove wsDirectMove;
WS_systemCmd wsSystemCmd;

// ----------------------- instances of AVR (RS232) message structures -----------------------

avrR_systemError avrRsystemError;
avrR_systemOk avrRsystemOk;

//----------------------------- receive message WS SERIALOPEN -----------------------------

void receiveMsgWsSerialOpen(unsigned char* msg,size_t len) {
	memcpy(&wsSerialOpen,msg,len);
	wsActionSerialOpen();
}

//---------------------------- receive message WS SERIALCLOSE -----------------------------

void receiveMsgWsSerialClose(unsigned char* msg,size_t len) {
	wsActionSerialClose();
}

//-------------------------- transmit WS SERIALSTATES message -----------------------------

void transmitMsgWsSerialStates() {
	unsigned char msg[sizeof(WS_serialStates) + 1];

	msg[0] = WSMSG_SERIALSTATES;
	msg[1] = (unsigned char) wsSerialStates.states;
	msg[2] = (unsigned char) wsSerialStates.codE;
	sendBinaryAllUsers(msg,sizeof(WS_serialStates) + 1);
}




//------------------------------ receive message WS MOVE ----------------------------------

void receiveMsgWsMove(unsigned char* msg,size_t len) {
	memcpy(&wsMove,msg,len);
	wsActionMove();
}

//------------------------------ receive message WS PARAM2AVR -----------------------------

void receiveMsgWsParam2Avr(unsigned char* msg,size_t len) {
	memcpy(&wsParam2avr,msg,len);
	wsActionParam2avr();
}

//------------------------------ receive message WS DIRECTMOVE ----------------------------

void receiveMsgWsDirectMove(unsigned char* msg,size_t len) {
	memcpy(&wsDirectMove,msg,len);
	wsActionDirectMove();
}

//------------------------------ receive message WS SYSTEMCMD -----------------------------

void receiveMsgWsSystemCmd(unsigned char* msg,size_t len) {
	memcpy(&wsSystemCmd,msg,len);
	wsActionSystemCmd();
}




//------------------------------ transmit WS COMMANDSTATUS message ---------------------

void transmitMsgWsCommandStatus() {
	unsigned char msg[sizeof(WS_commandStatus) + 1];
	
	msg[0] = WSMSG_COMMANDSTATUS;
	memcpy(&msg[1],&wsCommandStatus,sizeof(WS_commandStatus));
	sendBinaryAllUsers(msg,sizeof(WS_commandStatus) + 1);
}

//------------------------------ transmit WS PARAMAVR message --------------------------

void transmitMsgWsParamAvr() {
	unsigned char msg[sizeof(WS_paramAvr) + 1];

	msg[0] = WSMSG_PARAMAVR;
	memcpy(&msg[1],&wsParamAvr,sizeof(WS_paramAvr));
	sendBinaryAllUsers(msg,sizeof(WS_paramAvr) + 1);
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
		return;
		case WSMSG_MOVE:
			receiveMsgWsMove(&msg[1],len);
		return;
		case WSMSG_PARAM2AVR:
			receiveMsgWsParam2Avr(&msg[1],len);
		return;
		case WSMSG_DIRECTMOVE:
			receiveMsgWsDirectMove(&msg[1],len);
		return;
		case WSMSG_SYSTEMCMD:
			receiveMsgWsSystemCmd(&msg[1],len);
		return;
	}
}
