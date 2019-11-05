#include <util/atomic.h>
#include "atmega2560.h"
#include "atmega2560.h"
#include "atmega2560data.h"
#include "message.h"
#include "message_inline.h"
#include "feedback.h"
#include "feedback_inline.h"



inline void receive() {
	if (serial.codMsgDetectRX) {
		serial.codMsgDetectRX = 0;
		serial.msgLengthRX = 0;
		serial.countRX = 0;
		serial.enableTimeoutRX = 0;
		processMsg(serial.codMsgRX);
		serial.TXMsgReady = 1;
		serial.codMsgRX = 0;
	}
}

inline void trasmit() {
	if ((serial.TXMsgReady) && (!serial.TXBusy)) {
		if (serial.TXCodMsg == MSGT_COMMANDSTATUS) setCommandStatus();
		loadTxMsg(serial.TXCodMsg);
		serial.TXBusy = 1;
		serial.TXCount = 0;
		serial.TXMsgReady = 0;
		UDR0 = serial.TXCodMsg;
	}
	if ((serial.TXmsgTimeout)  && (!serial.TXBusy)) {
		msgTsystemOk.msgCodRX = 0;
		loadTxMsg(MSGT_SYSTEMERROR);
		serial.TXmsgTimeout = 0;
		serial.TXBusy = 1;
		serial.TXCount = 0;
		serial.TXMsgReady = 0;
		UDR0 = serial.TXCodMsg;
	}
}

inline void speedCalculation() {
	if ((encoderData.engineStop) && (encoderData.pwmHbrigde == 0)) {
		feedBackData.LastPosition = systemTimerFB.stepCounterB;
		systemTimerExtFB.ecoderMills = 0;
		systemTimerExtFB.cmdStop = 1;
		feedBackData.speed = 0;
	} else {
		if (systemTimerExtFB.deltaPosition) {
			systemTimerExtFB.cmdStop = 0;
			feedBackData.LastPosition = systemTimerExtFB.stepCounterB;
			feedBackData.ecoderMills = systemTimerExtFB.ecoderMills - 1;
			if (feedBackData.ecoderMills < 0) feedBackData.ecoderMills = 0;
			feedBackData.deltaMillsF = feedBackData.deltaMillsF + systemTimerExtFB.timeLapse;
			feedBackData.deltaMillsF = MAXTIMER_SYSTEMT - systemTimerExtFB.timeLapse;
			float timeMin = (feedBackData.ecoderMills + feedBackData.deltaMillsF * MULTIPLIER_FRACTIONAL_TIME) * 0.0012;
			if (timeMin) feedBackData.speed = (feedBackData.deltaPosition / timeMin);
			if (feedBackData.speed > 1750) feedBackData.speed = 1750;
			if (feedBackData.speed < 0) feedBackData.speed = 0;
		}
	}
}

inline void feedBack() {
	//initializes the parameters of the pid control after a disabling
	if ((!feedBackData.oldEnablePid) && (msgTdefaultData.enablePID)) initPID(); //sets the pid control variables after a disable
	feedBackData.oldEnablePid = msgTdefaultData.enablePID;
	if (!msgTdefaultData.directCmd) {
		if (msgTdefaultData.enablePID) pidControl(); //if enabled it performs the pid check, encoderData.pwmHbrigde is set by the control
		else encoderData.pwmHbrigde = feedBackData.speedTarget; //pid control disabled, encoderData.pwmHbrigde is set to target
	}
	//sends the pwm value to the pwm timer
	if (msgTdefaultData.directCmd) {
		systemReset.reset = 0;
		if (directMove.speedTarget) {
			CMD_SETRUN;   //macro with multiple instructions
		}
		else {
			CMD_SETSTOP;   //macro with multiple instructions
		}
		encoderData.pwmHbrigde = directMove.speedTarget; //direct move, encoderData.pwmHbrigde is set to target
	}
	VAR_DUTY_HBRIDGE = encoderData.pwmHbrigde; //set the pwm value in the H bridge
}

// loading of the next command
inline void changeCommand() {
	if (systemTimerFB.cmdEnd) {
		if  (systemTimerCall.moveCommandPresent) {
			CMD_SETSTART; 	//macro with multiple instructions
			systemTimerFB.pause = moveCommandData.pause;
			if (moveCommandData.pause) {
				CMD_SETPAUSE;   //macro with multiple instructions
			}
			else {
				CMD_SETRUN;   //macro with multiple instructions
			}
		}
	}
}

#define SETDIR_STOP_FORWARD STOP_MOTOR_HBRIDGE; SET_FORWARD_HBRIDGE; encoderData.engineStop = 0; encoderData.engineForward = 1; encoderData.engineBack = 0

#define SETDIR_STOP_REVERSE STOP_MOTOR_HBRIDGE; SET_REVERSE_HBRIDGE; encoderData.engineStop = 0; encoderData.engineForward = 0; encoderData.engineBack = 1

#define SETDIR_STOP STOP_MOTOR_HBRIDGE; encoderData.engineStop = 0; encoderData.engineForward = 0; encoderData.engineBack = 0

//set command direction
inline void setDirection() {
	if (msgTdefaultData.directCmd) {
		if (systemTimerFB.cmdStop) {
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				SETDIR_STOP;    //macro with multiple instructions
			}
		} else {
			if (directMove.forward != encoderData.engineForward) {
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
					if (directMove.forward) {
						SETDIR_STOP_FORWARD;   //macro with multiple instructions
						systemTimerFB.forward = 1;
					}
					else {
						SETDIR_STOP_REVERSE;   //macro with multiple instructions
						systemTimerFB.forward = 0;
					}
				}
			}
		}
	} else {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			if (systemTimerFB.cmdStop) {
				SETDIR_STOP;    //macro with multiple instructions
			} else {
				if ((systemTimerCall.moveCommandPresent) && (moveCommandData.forward != encoderData.engineForward)) {
					if (moveCommandData.forward) {
						SETDIR_STOP_FORWARD;   //macro with multiple instructions
						systemTimerFB.forward = 1;
					}
					else {
						SETDIR_STOP_REVERSE;   //macro with multiple instructions
						systemTimerFB.forward = 0;
					}
				}
			}
		}
	}
}

inline void enableDirectC() {
	msgTdefaultData.directCmd = 1;
	systemTimerCall.moveCommandPresent = 0;
	systemReset.reset = 1;
	STOP_MOTOR_HBRIDGE;
	encoderData.engineStop = 1;
	encoderData.engineForward = 0;
	encoderData.engineBack = 0;
	VAR_DUTY_HBRIDGE = 0;
	encoderData.cmdEnd = 1;
	encoderData.enableStopPosition = 0;
	encoderData.enableEndPosition = 0;
	CMD_SETSTOP;
	systemReset.commandReset = 0;
	encoderData.pwmHbrigde = directMove.speedTarget;
	VAR_DUTY_HBRIDGE = encoderData.pwmHbrigde;
}

inline void enableMoveC() {
	systemReset.commandReset = 0;
	systemReset.restoreMove = 0;
	msgTdefaultData.directCmd = 0;
	systemReset.reset = 0;
	CMD_SETEND;   //macro with multiple instructions
}

void init() {
	initializeMessage();
	initializeFeedBackData();
	configure();
}

int main(void) {
	init();
	while(1) {
		receive();
		if (systemReset.commandReset) {	// command reset (systemTimerCall.moveCommandPresent = 0;) and engine stop
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				enableDirectC();
			}
		}
		if (systemReset.restoreMove) {
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				enableMoveC();
			}
		}
		trasmit();
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			changeCommand();

			//makes a copy of the structure for external use from the interrupt
			uint8_t *dest = (uint8_t *) &systemTimerExtFB;	
			uint8_t *source = (uint8_t *) &systemTimerFB;
			for (uint8_t i = 0; i < sizeof(systemTimerFB); i++) dest[i] = source[i];
		}
		// sample the time interval of call to feedback
		if (systemTimerCall.FBRun) {
			systemTimerCall.FBInProgress = 1;	//while it is running it is not called
			systemTimerCall.FBRun = 0;
			setDirection();
			// speed calculation (encoderData.FBspeed/720) / (deltaTime/(60*1000)) *10
			speedCalculation();
			feedBack();
			systemTimerCall.FBInProgress = 0;
		}
	}
}

