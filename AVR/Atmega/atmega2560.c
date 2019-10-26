#include "atmega2560.h"
#include "atmega2560data.h"

// H Bridge

void configureHBridge() {

	encoderData.engineStop = 1;
	encoderData.engineForward = 0;
	encoderData.engineBack = 0;

	OUTPUTDIR_HBRIDGE;
	STOP_MOTOR_HBRIDGE;

	DIRPWM_HBRIDGE;

	SETAPWM_HBRIDGE;
	SETBPWM_HBRIDGE;

	MAXSTEP_HBRIDGE;
	VAR_DUTY_HBRIGE = 0;
	encoderData.pwmHbrigde = 0;
}

// encoder


ISR(INTERRUPTVECT_ENCODER) {	// called by the rising and falling edge of the selected pins
	register uint8_t newEncoderState = VALUE_ENCODER;
	register uint8_t testXor = encoderData.oldEncoderState ^ newEncoderState;
	switch (testXor) {
		case 1:
			switch (newEncoderState) {
				case 1:
				case 2:
					//decreases the position of the encoder position variable
					encoderData.deltaPosition--;
					// encoderData.timeLapse = read the counter register of the system timer
					encoderData.timeLapse = COUNTER_SYSTEM;
					break;
				default:
					//increases the position of the encoder position variable
					encoderData.deltaPosition++;
					// encoderData.timeLapse = read the counter register of the system timer
					encoderData.timeLapse = COUNTER_SYSTEM;
					break;
			}
			encoderData.oldEncoderState = newEncoderState;
			if (encoderData.enableEndPosition) {
				encoderData.endPosition--;
				if (!encoderData.endPosition) {
					encoderData.cmdEnd = 1;
					if (encoderData.enableStopPosition) {
						STOP_MOTOR_HBRIDGE;
						encoderData.engineStop = 1;
						encoderData.engineForward = 0;
						encoderData.engineBack = 0;
					}
				}
			}
		case 2:
			switch (newEncoderState) {
				case 1:
				case 2:
					//increases the position of the encoder position variable
					encoderData.deltaPosition++;
					// encoderData.timeLapse = read the counter register of the system timer
					encoderData.timeLapse = COUNTER_SYSTEM;
					break;
				default:
					//decreases the position of the encoder position variable
					encoderData.deltaPosition--;
					// encoderData.timeLapse = read the counter register of the system timer
					encoderData.timeLapse = COUNTER_SYSTEM;
					break;
			}
			encoderData.oldEncoderState = newEncoderState;
			if (encoderData.enableEndPosition) {
				encoderData.endPosition--;
				if (!encoderData.endPosition) {
					encoderData.cmdEnd = 1;
					if (encoderData.enableStopPosition) {
						STOP_MOTOR_HBRIDGE;
						encoderData.engineStop = 1;
						encoderData.engineForward = 0;
						encoderData.engineBack = 0;
					}
				}
			}
	}
}


void configureEncoder() {
	encoderData.enableStopPosition = 0;
	encoderData.enableEndPosition = 0;
	encoderData.endPosition = 0;
	encoderData.cmdEnd = 0;
	encoderData.deltaPosition = 0;
	encoderData.timeLapse = 0;

	SETPORT_ENCODER;
	INPUT_ENCODER;	// Set pin as input
	PULLUP_ENCODER;	// Activate PULL UP
	//SETINTERRUPT_ENCODER;
	encoderData.oldEncoderState = VALUE_ENCODER;
}

// system timer

//mega_systemTimerUART systemTimerSerial;


ISR (INTERRUPTVECT_SYSTEMT) {
	// change of parameters
	if (systemTimerCall.paramChange) {
		systemTimerCall.FBSampleTime = msgTdefaultData.FBSampleTime;
		systemTimerCall.paramChange = 0;
	}

	//end of command, loading of the next one
	if (encoderData.cmdEnd) {
		if (encoderData.enableStopPosition) {
			VAR_DUTY_HBRIGE = 0;
			encoderData.pwmHbrigde = 0;
			systemTimerFB.cmdStop = 1;
		}
		if (systemTimerCall.currentMoveCommand == 1) systemTimerCall.moveCommand1Present = 0;
		else systemTimerCall.moveCommand2Present = 0;
		systemTimerCall.currentMoveCommand = 0;
		encoderData.cmdEnd = 0;
		systemTimerFB.cmdRun = 0;
		systemTimerFB.cmdStart = 1;
		systemTimerFB.commandPointer = NULL;
	}
	if (systemTimerFB.cmdStart) {
		if (systemTimerCall.moveCommand1Present) {
			systemTimerCall.currentMoveCommand = 1;
			systemTimerFB.commandPointer = &moveCommand1Data;
			systemTimerFB.stepCounterB = systemTimerFB.commandPointer->nStepTarget;
			systemTimerFB.cmdStart = 0;
			systemTimerFB.cmdPause = 1;
		} else {
			if (systemTimerCall.moveCommand2Present) {
				systemTimerCall.currentMoveCommand = 2;
				systemTimerFB.commandPointer = &moveCommand2Data;
				systemTimerFB.stepCounterB = systemTimerFB.commandPointer->nStepTarget;
				systemTimerFB.cmdStart = 0;
				systemTimerFB.cmdPause = 1;
			}
		}
		systemTimerFB.nextCmd = systemTimerCall.moveCommand1Present + systemTimerCall.moveCommand2Present;
	}
	if (systemTimerFB.cmdPause) {
		if (!systemTimerFB.commandPointer->pause) {
			systemTimerFB.stepCounterB -= systemTimerFB.stepDrift;
			systemTimerFB.stepDrift = 0;
			systemTimerFB.cmdRun = 1;
			systemTimerFB.cmdPause = 0;
		}
	}

	// UART receive
	if (!serial.delayCountRX) {
		serial.delayCountRX = RECEIVE_DELAY_TIME;
		serial.replyMsgRunRX = 1;
		serial.codMsgDetectRX = 0;
	}
	if (serial.codMsgDetectRX) {
		serial.delayCountRX--;
	} else {
		if (serial.countRX > 0) {
			serial.codMsgDetectRX = 1;
		}
	}

	// position calculation
	systemTimerFB.position = encoderData.deltaPosition;
	if (encoderData.deltaPosition > 0) {
		//if the motor exceeds the end of command position
		if (systemTimerFB.stepCounterB < encoderData.deltaPosition) {
			systemTimerFB.stepDrift += systemTimerFB.stepCounterB - encoderData.deltaPosition;
			systemTimerFB.stepCounterB = 0;
		} else {
			systemTimerFB.deltaPosition = encoderData.deltaPosition;
			systemTimerFB.timeLapse = encoderData.timeLapse;
			systemTimerFB.stepCounterB -= encoderData.deltaPosition;
			systemTimerFB.forward = 1;
		}
	}
	if (encoderData.deltaPosition < 0) {
		uint8_t delta = -encoderData.deltaPosition;
		if (systemTimerFB.stepCounterB < delta) {
			systemTimerFB.stepDrift += systemTimerFB.stepCounterB - encoderData.deltaPosition;
			systemTimerFB.stepCounterB = 0;
		} else {
			systemTimerFB.deltaPosition = -encoderData.deltaPosition;
			systemTimerFB.timeLapse = encoderData.timeLapse;
			systemTimerFB.stepCounterB -= delta;
			systemTimerFB.forward = 0;
		}
	}
	// enables the end-of-position test
	if (systemTimerFB.stepCounterB <= MIN_STEP_END) {
		encoderData.enableEndPosition = 1;
		encoderData.endPosition = systemTimerFB.stepCounterB;
		if (systemTimerFB.commandPointer->enableStop) encoderData.enableStopPosition = 1;
	}
	// increments milliseconds between ecoder interrupts
	systemTimerFB.ecoderMills++;

	// sample the time interval of call to feedback and set the parameters
	if (! systemTimerCall.FBCount) {
		systemTimerCall.FBCount = systemTimerCall.FBSampleTime;
		if ((encoderData.engineStop) && (encoderData.pwmHbrigde == 0)) {
		systemTimerFB.ecoderMills = 0;
		encoderData.deltaPosition = 0;
	}
	if (systemTimerFB.cmdRun) {
		if (! systemTimerCall.FBInProgress) { //if it is not already running
				// encoder precalculation
				if (systemTimerFB.deltaPosition) {
					systemTimerFB.ecoderMills = 0;
					encoderData.deltaPosition = 0;
				}
				//enables the execution of the feedBack function
				systemTimerCall.FBRun = 1;
			}
		}
	}
	systemTimerCall.FBCount--;
}

void configureSystemTimer() {
	inizializesystemTimerFB();

	SETTIMER_SYSTEMT;
	SETINTERRUPT_SYSTEMT;
	OCRA_SYSTEMT;
}


// UART0



ISR(RX_VECT_SERIAL) {
	if ((serial.msgErrorRX) || (TESTERR_SERIAL)) {
		serial.buffRX[serial.countRX] = DATARXTX_SERIAL;
		serial.msgErrorRX = 1;
		serial.UCSRA = FLAG_SERIAL;
		return;
	}
	serial.buffRX[serial.countRX] = DATARXTX_SERIAL;
	if (!serial.countRX) serial.codMsgRX = serial.buffRX[0];
	serial.countRX++;
	if (serial.countRX >= MAX_BUFFRX) {
		serial.countRX = 0;
		serial.msgErrorRX = 1;
	}
}


ISR(TX_VECT_SERIAL) {
	if (serial.TXCount < serial.TXLenght) {
		DATARXTX_SERIAL = serial.TXBuff[serial.TXCount++];
	} else serial.TXBusy = 0;
}

void configureSerial() {

	serial.countRX = 0;
	serial.codMsgRX = 0;  // 0 = no message
	serial.msgErrorRX = 0;
	serial.codMsgDetectRX = 0;
	serial.replyMsgRunRX = 0;
	serial.delayCountRX = RECEIVE_DELAY_TIME;

	serial.TXEnable = 0;
	serial.TXMsgReady = 0; // message ready
	serial.TXMsgSysReady = 0; // message ready
	serial.TXCount = 0; // TX buffer counter
	serial.TXCodMsg = 0; // message code to be transmitted
	serial.TXSysCodMsg = 0; // message code to be transmitted
	serial.TXLenght = 0; // message length
	serial.TXBusy = 0;


	FLAG_SERIAL = 0x00;

	SETBAUDH_SERIAL = UBRRH_VALUE;
	SETBAUDL_SERIAL = UBRRL_VALUE;
#if USE_2X
	FLAG_SERIAL |= (1 << U2X_SERIAL);
#else
	FLAG_SERIAL &= ~(1 << U2X_SERIAL);
#endif

	SETOTHER_SERIAL;
	ENABLE_SERIAL;
}

void configure() {
	cli();
	configureHBridge();
	configureEncoder();
	configureSystemTimer();
	configureSerial();
	sei();
}

