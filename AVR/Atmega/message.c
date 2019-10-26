#include "message.h"
#include "atmega2560data.h"

msgT_systemError msgTsystemError;
msgT_systemOk msgTsystemOk;
msgT_systemTest msgTsystemTest;
msgT_commandStatus msgTcommandStatus;
msgT_defaultData msgTdefaultData;

uint8_t EEMEM EEPROMdefaulData[DIMT_DEFAULTDATA];

msgR_systemCmd msgRsystemCmd;
msgR_move moveCommand1Data;
msgR_move moveCommand2Data;
msgR_pidControl pidData;

void initializeMessage() {
	msgTsystemTest.test = 1;
	
	msgTdefaultData.enablePID = 0;		//enable PID control
	msgTdefaultData.Kp = 0.1;		//proportional pid coefficient
	msgTdefaultData.Ki = 0.1;		//integral pid coefficient
	msgTdefaultData.Kd = 0.1;		//pid derivative coefficient
	msgTdefaultData.FBSampleTime = FB_SAMPLE_TIME_START;	//sampling interval of the feed back function
	msgTdefaultData.ImaxSum = 1750;		//maximum value of the integral component of the pid control
	msgTdefaultData.IminSum = 0;		//minimum value of the integral component of the pid control
	msgTdefaultData.maxPWM = 1750;		//maximum value of pid control output
	msgTdefaultData.minPWM = 0;		//minimum value of pid control output
	msgTdefaultData.engineStartThreshold = 10;	//engine starting threshold
}

