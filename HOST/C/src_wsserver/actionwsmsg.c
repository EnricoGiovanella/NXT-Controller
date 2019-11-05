#include <stdio.h>
#include <string.h>
#include "include/actionwsmsg.h"
#include "include/message.h"
#include "include/configurers232.h"
#include "include/usbrs232.h"

//--------------------------- action on WS SERIALOPEN message ---------------------------

void wsActionSerialOpen() {
	fprintf(stderr, "\nwsSerialOpen.device %s wsSerialOpen.baud %u wsSerialOpen.parity %u wsSerialOpen.stop %u wsSerialOpen.data %u\n", wsSerialOpen.device, wsSerialOpen.baud, wsSerialOpen.parity, wsSerialOpen.stop, wsSerialOpen.data);
	setRs232(wsSerialOpen.device, wsSerialOpen.baud, wsSerialOpen.parity, wsSerialOpen.stop, wsSerialOpen.data);
	openRs232();
}

//--------------------------- action on WS SERIALCLOSE message --------------------------

void wsActionSerialClose() {
	closeRs232();
	fprintf(stderr, "\nwsSerialClose\n");
}

//--------------------------- action on WS MOVE message --------------------------

void wsActionMove() {
	//transmitRs232(uint8_t instant,uint8_t wsCod = fifo,uint8_t msgCod,uint8_t* msg,size_t len)
	transmitRs232(!wsMove.fifo,wsMove.codCmd,AVRT_MOVE,(uint8_t*) &wsMove,DIM_MOVE);
	fprintf(stderr, "\nwsMove.speedTarget %u wsMove.nStepTarget %u\n",
	wsMove.speedTarget,wsMove.nStepTarget);
	fprintf(stderr, "wsMove.forward %u wsMove.enableStop %u wsMove.pause %u\n", 
	wsMove.forward,wsMove.enableStop,wsMove.pause);
	fprintf(stderr, "wsMove.millsPause %u wsMove.fifo %u  wsMove.codCmd %u\n",
	wsMove.millsPause,wsMove.fifo,wsMove.codCmd);
}

//--------------------------- action on WS PARAM2AVR message --------------------------

void wsActionParam2avr() {
	//adapts the pid coefficients to sampling
	if(wsParam2avr.FBSampleTime != wsParamAvr.FBSampleTime) {
    		float sampleTimeInSec = (float) wsParamAvr.FBSampleTime/1000;
		if(sampleTimeInSec > 0) {
			wsParam2avr.Ki = wsParam2avr.Ki * sampleTimeInSec;
			wsParam2avr.Kd = wsParam2avr.Kd / sampleTimeInSec;
		}
		if((wsParam2avr.FBSampleTime > 0) && (wsParamAvr.FBSampleTime > 0)) {
    			float ratio = wsParam2avr.FBSampleTime  / wsParamAvr.FBSampleTime;
    			wsParam2avr.Ki *= ratio;
   			wsParam2avr.Kd /= ratio;
		}
	}
	memcpy(&wsParamAvr,&wsParam2avr,DIM_PIDCONTROL);
	//transmitRs232(uint8_t instant,uint8_t wsCod = fifo,uint8_t msgCod,uint8_t* msg,size_t len)
	transmitRs232(!wsParam2avr.fifo,0,AVRT_PIDCONTROL,(uint8_t*) &wsParam2avr,DIM_PIDCONTROL);

	fprintf(stderr, "\nwsParam2avr.enablePID %u wsParam2avr.Kp %f wsParam2avr.Ki %f wsParam2avr.Kd %f\n",
	wsParam2avr.enablePID,wsParam2avr.Kp,wsParam2avr.Ki,wsParam2avr.Kd);
	fprintf(stderr, "wsParam2avr.FBSampleTime %u wsParam2avr.ImaxSum %d wsParam2avr.IminSum %d\n",
	wsParam2avr.FBSampleTime,wsParam2avr.ImaxSum,wsParam2avr.IminSum);
	fprintf(stderr, "wsParam2avr.maxPWM %d wsParam2avr.minPWM %d wsParam2avr.engineStartThreshold %u\n",
	wsParam2avr.maxPWM,wsParam2avr.minPWM,wsParam2avr.engineStartThreshold);
	fprintf(stderr, "wsParam2avr.directCmd %u wsParam2avr.fifo%u\n",wsParam2avr.directCmd,wsParam2avr.fifo);
}

//--------------------------- action on WS DIRECTMOVE message --------------------------

void wsActionDirectMove() {
	fprintf(stderr, "\nwsDirectMove.speedTarget %u wsDirectMove.forward %u\n",
	wsDirectMove.speedTarget,wsDirectMove.forward);
	//transmitRs232(uint8_t instant,uint8_t wsCod = fifo,uint8_t msgCod,uint8_t* msg,size_t len)
	transmitRs232(1,0,AVRT_DIRECTMOVE,(uint8_t*) &wsDirectMove,DIM_DIRECTMOVE);
}

//--------------------------- action on WS SYSTEMCMD message --------------------------

void printSystemCmdCmd(int tipo) {
	if(tipo) {
		fprintf(stderr, "\n---------------------------------------------\n");
		fprintf(stderr, "\nwsSystemCmd.cmd cod = %u subCmd = %u\n",WSMSG_SYSTEMCMD,wsSystemCmd.cmd);
		fprintf(stderr, "\n web --> avr cod = %u subCmd = %d/n",AVRT_SYSTEMCMD,wsSystemCmd.cmd);
	} else {
		fprintf(stderr, "\n---------------------------------------------\n");
		fprintf(stderr, "\n fifo --> avr cod = %u subCmd = %d/n", AVRT_SYSTEMCMD,wsSystemCmd.cmd);
	}
}

void wsActionSystemCmd() {
	if(wsSystemCmd.cmd == 6) resetFIFOSend();
	//printSystemCmdCmd(1);
	//transmitRs232(uint8_t instant,uint8_t wsCod = fifo,uint8_t msgCod,uint8_t* msg,size_t len)
	transmitRs232(1,0,AVRT_SYSTEMCMD,(uint8_t*) &wsSystemCmd,DIM_SYSTEMCMD);
}


