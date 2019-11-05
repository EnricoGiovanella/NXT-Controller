#include <string.h>
#include "include/actionmsgavr.h"
#include "include/message.h"
#include "include/usbrs232.h"
//#include "include/timertx.h"


//--------------------------- action on AVR (RS232) SYSTEMOK message -------------------

void actionSystemOk(uint8_t* buf,size_t len) {
	memcpy(&avrRsystemOk,buf,DIM_SYSTEMOK);
	setTXDataMsgOk(1);
//	serialTxData.msgOk = 1;
	fprintf(stderr,"\nmsg ok msgCodRX %u subMsgCodRX %u subMsgCodRX1 %u DIMR_SYSTEMOK %lu\n\n",
		avrRsystemOk.msgCodRX,avrRsystemOk.subMsgCodRX,avrRsystemOk.subMsgCodRX1,DIM_SYSTEMOK);
}


//--------------------------- action on AVR (RS232) SYSTEMERROR message -------------------


void actionSystemError(uint8_t* buf,size_t len) {
	memcpy(&avrRsystemError,buf,DIM_SYSTEMERROR);
	///if(serialTxData.msgReadyBuf)
	if(isTXDataMsgReadyBuf()) setTXDataReplyMsg(1);
	fprintf(stderr,"\nmsg errore UCSRA %u\n\n",avrRsystemError.UCSRA);
	setTXDataMsgOk(0);
	//serialTxData.msgOk = 0;
}


//--------------------------- action on RS232 COMMANDSTATUS message ----------------------

int isMoveCmdNotPresent() {
	if(wsCommandStatus.status & STATUS_PRESENT) return 0;
	else return 1;
}
void initTestCommandStatus() {
	wsCommandStatus.speed = 0;
	wsCommandStatus.position = 0;
	wsCommandStatus.pwmHbridge = 0;  //pwm input value of bridge H
	wsCommandStatus.forward = 2;  //direction of rotation of the motor 1 = increase of the steps 0 = decrease of the steps
	wsCommandStatus.pTerm = 0;    //proportional part of the pid control
	wsCommandStatus.iTerm = 0;    //integral part of the pid control
	wsCommandStatus.dTerm = 0;    //derived part of the pid contr
	wsCommandStatus.status = 0;   //status as described above
	wsCommandStatus.status |= STATUS_STOP;
	wsCommandStatus.directCmd = 0; //direct command to the motor by the host
	wsCommandStatus.codCmd = 0; 	//message numeric code
}


void dispatchCommandStatus() {
	transmitMsgWsCommandStatus();
}

void actionCommandStatus(uint8_t* buf,size_t len) {
	memcpy(&wsCommandStatus,buf,len);


/*
	fprintf(stderr,"\n\nwsCommandStatus.position %u wsCommandStatus.nStepLeft %u wsCommandStatus.driftNstep %d wsCommandStatus.pwmHbridge %u\n",
	wsCommandStatus.position,
	wsCommandStatus.nStepLeft,
	wsCommandStatus.driftNstep,
	wsCommandStatus.pwmHbridge);
	fprintf(stderr,"wsCommandStatus.forward %u wsCommandStatus.pTerm %d wsCommandStatus.iTerm %d wsCommandStatus.dTerm %d\n",
	wsCommandStatus.forward,
	wsCommandStatus.pTerm,
	wsCommandStatus.iTerm,
	wsCommandStatus.dTerm);
	fprintf(stderr,"wsCommandStatus.status %u wsCommandStatus.directCmd %u wsCommandStatus.codCmd %u\n",
	wsCommandStatus.status,
	wsCommandStatus.directCmd,
	wsCommandStatus.codCmd);*/

	// position = workCommandStatus.position

	// copy command status struct in internal work struct
	//memcpy(&workCommandStatus,&avrRcommandStatus,len);

	//if(position != avrRcommandStatus.position) 
}

//--------------------------- action on RS232 PARAMAVR message --------------------------

void sendTestDefaultData() {
	wsParamAvr.enablePID = 0;
	wsParamAvr.Kp = 1.1;
	wsParamAvr.Ki = 1.2;
	wsParamAvr.Kd = 1.3;
	wsParamAvr.FBSampleTime = 20;
	wsParamAvr.ImaxSum = 1750;
	wsParamAvr.IminSum = 10;
	wsParamAvr.maxPWM = 1700;
	wsParamAvr.minPWM = 20;
	wsParamAvr.engineStartThreshold = 400;
	wsParamAvr.directCmd = 1;
	transmitMsgWsParamAvr();
}

void actionDefaultData(uint8_t* buf,size_t len) {
	memcpy(&wsParamAvr,buf,len);
	fprintf(stderr, "\nwsParamAvr.enablePID %u wsParamAvr.Kp %f wsParamAvr.Ki %f wsParamAvr.Kd %f\n",
	wsParamAvr.enablePID,wsParamAvr.Kp,wsParamAvr.Ki,wsParamAvr.Kd);
	fprintf(stderr, "wsParamAvr.FBSampleTime %u wsParamAvr.ImaxSum %d wsParamAvr.IminSum %d\n",
	wsParamAvr.FBSampleTime,wsParamAvr.ImaxSum,wsParamAvr.IminSum);
	fprintf(stderr, "wsParamAvr.maxPWM %d wsParamAvr.minPWM %d wsParamAvr.engineStartThreshold %u\n",
	wsParamAvr.maxPWM,wsParamAvr.minPWM,wsParamAvr.engineStartThreshold);
	fprintf(stderr, "wsParamAvr.directCmd %u wsParamAvr.fifo%u\n",wsParamAvr.directCmd,wsParamAvr.fifo);
	transmitMsgWsParamAvr();
}
