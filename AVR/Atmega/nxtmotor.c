#include <util/atomic.h>
#include "atmega2560.h"
#include "atmega2560.h"
#include "atmega2560data.h"
#include "message.h"
#include "message_inline.h"
#include "feedback.h"
#include "feedback_inline.h"



inline void speedCalculation() {
	if ((encoderData.engineStop) && (encoderData.pwmHbrigde == 0)) {
		feedBackData.LastPosition = systemTimerFB.stepCounterB;
		systemTimerExtFB.ecoderMills = 0;
		systemTimerExtFB.cmdStop = 1;
		//encoderData.deltaPosition = 0;
		feedBackData.speed = 0;
	} else {
		if (systemTimerExtFB.deltaPosition) {
			systemTimerExtFB.cmdStop = 0;
			feedBackData.LastPosition = systemTimerExtFB.stepCounterB;
			feedBackData.ecoderMills = systemTimerExtFB.ecoderMills - 1;
			feedBackData.deltaMillsF = feedBackData.deltaMillsF + systemTimerExtFB.timeLapse;
			feedBackData.deltaMillsF = MAXTIMER_SYSTEMT - systemTimerExtFB.timeLapse;
			//systemTimerFB.ecoderMills = 0;
			float timeMin = (feedBackData.ecoderMills + feedBackData.deltaMillsF * MULTIPLIER_FRACTIONAL_TIME) * 0.0012;
			if (timeMin) feedBackData.speed = (feedBackData.deltaPosition / timeMin);
		}
	}
}


inline void feedBack() {
	//retrieves the necessary data
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		uint8_t *dest = (uint8_t *) &systemTimerExtFB;
		uint8_t *source = (uint8_t *) &systemTimerFB;
		for (uint8_t i = 0; i < sizeof(systemTimerFB); i++) dest[i] = source[i];
		systemTimerExtFB.nextCmd = systemTimerCall.moveCommand1Present + systemTimerCall.moveCommand2Present;
		if (systemTimerExtFB.commandPointer != NULL) {
			feedBackData.speedTarget = systemTimerExtFB.commandPointer->speedTarget;
		}
	}
	// speed calculation (encoderData.FBspeed/720) / (deltaTime/(60*1000)) *10
	speedCalculation();
	//initializes the parameters of the pid control after a disabling
	if ((!feedBackData.oldEnablePid) && (msgTdefaultData.enablePID)) initPID();
	feedBackData.oldEnablePid = msgTdefaultData.enablePID;
	if (msgTdefaultData.enablePID) pidControl();
	else encoderData.pwmHbrigde = feedBackData.speedTarget;
	//sends the pwm value to the pwm timer
	VAR_DUTY_HBRIGE = encoderData.pwmHbrigde;
	//prepare status message for sending
	if (serial.TXEnable) {
		setCommandStatus();
		serial.TXMsgReady = 1;
		serial.TXCodMsg = MSGT_COMMANDSTATUS;
	}
}

inline void receive() {
	if (serial.replyMsgRunRX) {
		if (serial.msgErrorRX) {
			serial.msgErrorRX = 0;
			serial.TXSysCodMsg = loadTxMsg(MSGT_SYSTEMERROR);
			if (serial.TXSysCodMsg) serial.TXMsgSysReady = 1;
		} else {
			uint8_t flagOk = processMsg(serial.TXCodMsg);
			switch (flagOk) {
				case 0:
					serial.TXSysCodMsg = loadTxMsg(MSGT_SYSTEMERROR);
					if (serial.TXSysCodMsg) serial.TXMsgSysReady = 1;
					break;
				case 1:
					serial.TXSysCodMsg = loadTxMsg(MSGT_SYSTEMOK);
					if (serial.TXSysCodMsg) serial.TXMsgSysReady = 1;
					break;
			}
		}
	}
}

inline void trasmit() {
	if (serial.TXMsgSysReady) {

		while (!(UCSR0A & (1 << UDRE0))) {};

		serial.TXMsgSysReady = 0;
		uint8_t codSysMsg = loadTxMsg(serial.TXSysCodMsg);
		if (codSysMsg) {
			serial.TXCount = 0;
			serial.TXSysCodMsg = 0;
			serial.TXBusy = 1;

			UDR0 = codSysMsg;
		}
	} else {
		if (serial.TXMsgReady) {

			while (!(UCSR0A & (1 << UDRE0))) {};

			serial.TXMsgReady = 0;
			uint8_t codMsg = loadTxMsg(serial.TXCodMsg);
			if (codMsg) {
				serial.TXCount = 0;
				serial.TXCodMsg = 0;
				serial.TXBusy = 1;
				UDR0 = serial.TXCodMsg;
			}
		}
	}
}

uint8_t commandReset = 0;


int main(void) {

	initializeMessage();
	initializeFeedBackData();
	configure();

	while(1) {
		// put your main code here, to run repeatedly:
		receive();
		// sample the time interval of call to feedback
		if (systemTimerCall.FBRun) {
			systemTimerCall.FBInProgress = 1;
			systemTimerCall.FBRun = 0;
			//set command direction
			if (systemTimerExtFB.cmdRun) {
				if (systemTimerExtFB.commandPointer != NULL) {
					if (systemTimerExtFB.commandPointer->forward != encoderData.engineForward) {
						ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
							STOP_MOTOR_HBRIDGE;
							SET_FORWARD_HBRIDGE;
							encoderData.engineStop = 0;
							encoderData.engineForward = 1;
							encoderData.engineBack = 0;
						}
					} else {
						ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
							STOP_MOTOR_HBRIDGE;
							SET_REVERSE_HBRIDGE;
							encoderData.engineStop = 0;
							encoderData.engineForward = 0;
							encoderData.engineBack = 1;
						}
					}
				}
			}
			feedBack();
			systemTimerCall.FBInProgress = 0;
		}
		trasmit();
		if (commandReset) {
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				STOP_MOTOR_HBRIDGE;
				encoderData.engineStop = 1;
				encoderData.engineForward = 0;
				encoderData.engineBack = 0;
				encoderData.cmdEnd = 1;
				systemTimerCall.moveCommand1Present = 0;
				systemTimerCall.moveCommand2Present = 0;
				VAR_DUTY_HBRIGE = 0;
				feedBackData.reset = 1;
				commandReset = 0;
			}
		}
	}
}



