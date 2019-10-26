#include "atmega2560data.h"

mega_serial serial;
mega_systemTimerCall systemTimerCall;
mega_systemTimerFB systemTimerFB;
mega_systemTimerFB systemTimerExtFB;
mega_encoder encoderData;

void inizializesystemTimerFB() {
	systemTimerFB.commandPointer = NULL;	//pointer to the current command
	systemTimerFB.stepCounterB = 0;		//step counter backwards
	systemTimerFB.stepDrift = 0;		//steps exceeding the command steps
	systemTimerFB.position = 0;	//engine position
	systemTimerFB.ecoderMills = 0;	//milliseconds elapsed between two steps
	//volatile uint16_t deltaPreMillsF; //fraction of milliseconds
	systemTimerFB.forward = 1;	//direction calculated from the encoder data
	systemTimerFB.timeLapse = 0;	//time interval = value of TCNT register (1/16000 milliseconds)
	systemTimerFB.deltaPosition = 0;//number of encoder steps between the sampling interval of the system time routine (positive forward, negative back)
	systemTimerFB.cmdStop = 0;	//there was an engine stop
	systemTimerFB.cmdRun = 0;	//the current command is running
	systemTimerFB.cmdStart = 1;	//the current command has been acquired
	systemTimerFB.cmdPause = 0;	//the current command is paused
	systemTimerFB.nextCmd = 0;	//2 indicates that the next command exists
  
  uint8_t *dest = (uint8_t *) &systemTimerExtFB;
  uint8_t *source = (uint8_t *) &systemTimerFB;
  for(uint8_t i=0; i<sizeof(systemTimerFB); i++) dest[i] = source[i];

	systemTimerCall.FBSampleTime = msgTdefaultData.FBSampleTime;  //interval in milliseconds of activation
	systemTimerCall.FBCount = systemTimerCall.FBSampleTime; //counter to activate the feed back function
	systemTimerCall.FBRun = 0;	//flag that activates the feed back function
	systemTimerCall.FBInProgress = 0; //flag indicating that the function is running
	systemTimerCall.paramChange = 0;	//the parameters received have changed
	systemTimerCall.moveCommand1Present = 0;  //indicates the presence of a command in moveCommand1
	systemTimerCall.moveCommand2Present = 0;  //indicates the presence of a command in moveCommand2
	systemTimerCall.currentMoveCommand = 0;  //indicates which structure is used as the current command
}

