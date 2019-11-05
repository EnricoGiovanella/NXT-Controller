#include "message.h"
#include "atmega2560data.h"

msgT_systemError msgTsystemError;
msgT_systemOk msgTsystemOk;
//msgT_systemTest msgTsystemTest;
msgT_commandStatus msgTcommandStatus;
msgT_defaultData msgTdefaultData;

uint8_t EEMEM EEPROMdefaulData[DIMT_DEFAULTDATA];

msgR_systemCmd msgRsystemCmd;
msgR_move moveCommandData;

msgR_directMove directMove;

void initializeMessage() {
	msgTdefaultData.enablePID = 0;    //enable PID control
	msgTdefaultData.Kp = 1.0;   //proportional pid coefficient
	msgTdefaultData.Ki = 1.0;   //integral pid coefficient
	msgTdefaultData.Kd = 1.0;   //pid derivative coefficient
	msgTdefaultData.FBSampleTime = FB_SAMPLE_TIME_START;  //sampling interval of the feed back function
	msgTdefaultData.ImaxSum = 1750;   //maximum value of the integral component of the pid control
	msgTdefaultData.IminSum = 0;    //minimum value of the integral component of the pid control
	msgTdefaultData.maxPWM = 1750;    //maximum value of pid control output
	msgTdefaultData.minPWM = 0;   //minimum value of pid control output
	msgTdefaultData.engineStartThreshold = 100;  //engine starting threshold
	msgTdefaultData.directCmd = 2;  //direct command to the motor by the host
	msgTdefaultData.fifo = 0; //1 fifo 0 direct

	msgTcommandStatus.speed = 0;
	msgTcommandStatus.position = 0;
	msgTcommandStatus.nStepLeft = 0;
	msgTcommandStatus.driftNstep = 0;
	msgTcommandStatus.pwmHbridge = 0;
	msgTcommandStatus.forward = 2;
	msgTcommandStatus.pTerm = 0;
	msgTcommandStatus.iTerm = 0;
	msgTcommandStatus.dTerm = 0;
	msgTcommandStatus.status &= ~STATUS_PAUSE;
	msgTcommandStatus.status &= ~STATUS_PRESENT;
	msgTcommandStatus.status &= ~STATUS_RUN;
	msgTcommandStatus.status |= STATUS_STOP;;
	msgTcommandStatus.status |= STATUS_END;
	msgTcommandStatus.status |= STATUS_RESET;

	directMove.speedTarget = 0;  //target motor speed (0 1750)
	directMove.forward = 0; //direction of rotation of the motor 1 = increase of the steps 0 = decrease of the steps
}

