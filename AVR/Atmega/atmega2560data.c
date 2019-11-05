#include "atmega2560data.h"

mega_serial serial;
mega_systemTimerCall systemTimerCall;
mega_systemTimerFB systemTimerFB;
mega_systemTimerFB systemTimerExtFB;
mega_encoder encoderData;
mega_systemReset systemReset;

void inizializesystemTimerFB() {
	systemTimerFB.stepCounterB = 0;   //step counter backwards
	systemTimerFB.stepDrift = 0;    //steps exceeding the command steps
	systemTimerFB.position = 0; //engine position
	systemTimerFB.ecoderMills = 0;  //milliseconds elapsed between two steps
	systemTimerFB.forward = 1;  //direction calculated from the encoder data
	systemTimerFB.timeLapse = 0;  //time interval = value of TCNT register (1/16000 milliseconds)
	systemTimerFB.deltaPosition = 0;//number of encoder steps between the sampling interval of the system time routine (positive forward, negative back)
	systemTimerFB.pause = 0;	//1 command paused 0 immediate command
	systemTimerFB.pauseCount = 0; //pause counter backwards
	systemTimerFB.cmdStop = 1;  //there was an engine stop
	systemTimerFB.cmdEnd = 1;  //end of the current command
	systemTimerFB.cmdRun = 0; //the current command is running
	systemTimerFB.cmdStart = 0; //the current command is present
	systemTimerFB.cmdPause = 0; //the current command is paused

	systemTimerCall.FBSampleTime = msgTdefaultData.FBSampleTime;  //interval in milliseconds of activation
	systemTimerCall.FBCount = systemTimerCall.FBSampleTime; //counter to activate the feed back function
	systemTimerCall.FBRun = 0;  //flag that activates the feed back function
	systemTimerCall.FBInProgress = 0; //flag indicating that the function is running
	systemTimerCall.moveCommandPresent = 0;  //indicates the presence of a command in moveCommand
	systemTimerCall.paramChange = 0;  //the parameters received have changed

	systemReset.commandReset = 0;
	systemReset.reset = 0;
  systemReset.restoreMove =0;
}

void inizializeSerial() {
	serial.countRX = 0;     // receive byte counter
	serial.codMsgRX = 0;    // 0 = no message
	serial.msgLengthRX = 0; //length of the message or 0
	serial.codMsgDetectRX = 0; //flag detect receive message
	serial.delayCountRX = RECEIVE_DELAY_TIME;  // delay timeout receive message
	serial.enableTimeoutRX = 0;   //enable timeout receive message counter
	serial.msgErrorRX = 0;    //flag error receive
	serial.UCSRA = 0;   //status register of the UART
	serial.replyMsgRunRX = 0; //flag run function replyMsg()

	serial.TXCount = 0; // TX buffer counter
	serial.TXEnable = 0; // enable transmission used in
	serial.TXMsgReady = 0; // message ready
	serial.TXCodMsg = 0; // message code to be transmitted
	serial.TXMsgSysReady = 0; // message system ready
	serial.TXSysCodMsg = 0; // message ystem code to be transmitted
	serial.TXLenght = 0; // message length
	serial.TXBusy = 0;  //Tx busy

	serial.TXmsgTimeout = 0; //timout trasmitter
}


