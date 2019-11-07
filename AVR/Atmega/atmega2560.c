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
	VAR_DUTY_HBRIDGE = 0;
	encoderData.pwmHbrigde = 0;
}

// encoder


ISR(INTERRUPTVECT_ENCODER) {  // called by the rising and falling edge of the selected pins
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
			break;
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
			break;
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
	INPUT_ENCODER;  // Set pin as input
	PULLUP_ENCODER; // Activate PULL UP
	SETINTERRUPT_ENCODER;
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

	// UART receive
	if (serial.enableTimeoutRX) {
		if (!serial.delayCountRX) {
			serial.enableTimeoutRX = 0;
			serial.TXmsgTimeout = 1;
			serial.delayCountRX = RECEIVE_DELAY_TIME;
		} else serial.delayCountRX--;
	}

	//end of command
	if (!msgTdefaultData.directCmd) {
		if (encoderData.cmdEnd) {
			if (encoderData.enableStopPosition) {
				VAR_DUTY_HBRIDGE = 0;
				encoderData.pwmHbrigde = 0;
				//systemTimerFB.cmdStop = 1;
				CMD_SETSTOP;	//macro with multiple instructions
			} else {
				CMD_SETEND;   //macro with multiple instructions
			}
			encoderData.cmdEnd = 0;
			encoderData.enableStopPosition = 0;
			encoderData.enableEndPosition = 0;
			systemTimerCall.moveCommandPresent = 0;
			//serial.TXMsgCmdStatus = 1;
		}
	}

	// timer pause
	if (!msgTdefaultData.directCmd) {
		if (systemTimerFB.pause) {
			systemTimerFB.pause = 0;
			systemTimerFB.pauseCount = moveCommandData.millsPause;
		}
		if (systemTimerFB.cmdPause) {
			if (!systemTimerFB.pauseCount) {
				CMD_SETRUN; //macro with multiple instructions
			}
			systemTimerFB.pauseCount--;
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
	// increments milliseconds between ecoder interrupts
	systemTimerFB.ecoderMills++;

	// enables the end-of-position test
	if (!msgTdefaultData.directCmd) {
		if ((systemTimerFB.stepCounterB <= MIN_STEP_END) && (systemTimerCall.moveCommandPresent) && (!encoderData.enableEndPosition)) {
			encoderData.enableEndPosition = 1;
			encoderData.endPosition = systemTimerFB.stepCounterB;
			if (moveCommandData.enableStop) encoderData.enableStopPosition = 1;
		}
	}

	// sample the time interval of call to feedback and set the speed parameters
	if (! systemTimerCall.FBCount) {
		systemTimerCall.FBCount = systemTimerCall.FBSampleTime;
		systemTimerCall.FBRun = 1;
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

	/*

	*/
}


void configureSystemTimer() {
	inizializesystemTimerFB();

	SETTIMER_SYSTEMT;
	SETINTERRUPT_SYSTEMT;
	OCRA_SYSTEMT;
}


// UART0

ISR(RX_VECT_SERIAL) {
	if (TESTERR_SERIAL) {
		serial.buffRX[0] = DATARXTX_SERIAL;
		serial.msgErrorRX = 1;
		serial.UCSRA = FLAG_SERIAL;
		serial.codMsgRX = 0;
		serial.enableTimeoutRX = 1;
		serial.delayCountRX = RECEIVE_DELAY_TIME;
	}
	if (!serial.codMsgRX) {
		serial.enableTimeoutRX = 1;
		serial.delayCountRX = RECEIVE_DELAY_TIME;
		serial.codMsgRX = DATARXTX_SERIAL;
		switch (serial.codMsgRX) {
			case MSGR_SYSTEMCMD:
				serial.msgLengthRX = DIMR_SYSTEMCMD;
				break;
			case MSGR_MOVE:
				serial.msgLengthRX = DIMR_MOVE;
				break;
			case MSGR_PIDCONTROL:
				serial.msgLengthRX = DIMR_PIDCONTROL;
				break;
      case MSGR_DIRECTMOVE:
        serial.msgLengthRX = DIMR_DIRECTMOVE;
        break;
		}
	} else {
		serial.buffRX[serial.countRX] = DATARXTX_SERIAL;
		serial.countRX++;
	}
	if (serial.countRX >= serial.msgLengthRX) serial.codMsgDetectRX = 1;
}


ISR(TX_VECT_SERIAL) {
	if (serial.TXCount < serial.TXLenght) {
		DATARXTX_SERIAL = serial.TXBuff[serial.TXCount++];
	} else serial.TXBusy = 0;
}

void configureSerial() {

	inizializeSerial();

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

