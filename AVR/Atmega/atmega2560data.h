#ifndef ATMEGA2560DATA_H
#define ATMEGA2560DATA_H

#include <stdint.h>
#include "message.h"


/////////////////////////////////////////////////////////////////////////////////
//data structure used by the encoder interrupt routine and the pwm timer


typedef struct  __attribute__((__packed__)) {
	volatile uint8_t oldEncoderState; //previous state of the encoder inputs
	volatile uint8_t enableStopPosition;  //enables motor stop if command provides (1)
	volatile uint8_t enableEndPosition; //enables the control of the end of the command that is when moveCommandData.nStepTarget through endPosition set by the system timer routine (1)
	volatile int8_t endPosition;  //steps missing at the end of the command
	volatile uint8_t cmdEnd;  //raggiunta la fine del commando
	volatile int8_t deltaPosition;  //number of encoder steps between the sampling interval of the system time routine (positive forward, negative back)
	volatile uint16_t timeLapse;  //time interval = value of TCNT register (1/16000 milliseconds)
	//variables used by bridge H
	volatile uint16_t pwmHbrigde; //pwm value of bridge H
	volatile uint8_t engineStop;  //flag 1 = the motor is stopped
	volatile uint8_t engineForward; //flag 1 = the motor turns forward
	volatile uint8_t engineBack;  //flag 1 = the engine turns bac
} mega_encoder;

extern mega_encoder encoderData;

/////////////////////////////////////////////////////////////////////////////////
//data structure used for transmission and in the system timer interrupt routine


#define MAX_BUFFRX 40
#define MAX_BUFFTX 40
#define RECEIVE_DELAY_TIME 2

typedef struct  __attribute__((__packed__)) {
	volatile uint8_t buffRX[MAX_BUFFRX];	// receive buffer
	volatile uint8_t countRX;     // receive byte counter
	volatile uint8_t codMsgRX;    // 0 = no message
	volatile uint8_t msgLengthRX; //length of the message or 0
	volatile uint8_t codMsgDetectRX; //flag detect receive message
	volatile uint8_t delayCountRX;  // delay respose message counter
	volatile uint8_t enableTimeoutRX;   //enable timeout receive message
	volatile uint8_t msgErrorRX;    //flag error receive
	volatile uint8_t UCSRA;   //status register of the UART
	volatile uint8_t replyMsgRunRX; //flag run function replyMsg()

	volatile uint8_t TXBuff[MAX_BUFFTX]; // transmission buffer
	volatile uint8_t TXCount; // TX buffer counter
	volatile uint8_t TXEnable; // enable transmission used in trasmitCmdStatus()
	volatile uint8_t TXMsgReady; // message ready
	volatile uint8_t TXMsgCmdStatus;//command status message to send
	volatile uint8_t TXCodMsg; // message code to be transmitted
	volatile uint8_t TXMsgSysReady; // message system ready
	volatile uint8_t TXSysCodMsg; // message ystem code to be transmitted
	volatile uint8_t TXLenght; // message length
	volatile uint8_t TXBusy;  //Tx busy

	volatile uint8_t TXmsgTimeout;

} mega_serial;

//global serial management variable
extern mega_serial serial;

/////////////////////////////////////////////////////////////////////////////////
//data structure used in the system timer interrupt routine for the feedback function


#define FB_SAMPLE_TIME_START 2
#define MIN_STEP_END 20

typedef struct  __attribute__((__packed__)) {
	volatile uint16_t FBCount; //counter to activate the feed back function
	volatile uint8_t FBRun; //flag that activates the feed back function
	volatile uint8_t FBInProgress; //flag indicating that the function is running
	volatile uint8_t FBSampleTime;  //interval in milliseconds of activation
	volatile uint8_t moveCommandPresent;  //indicates the presence of a command in moveCommand
	volatile uint8_t paramChange; //the parameters received have changed
} mega_systemTimerCall;

//global feed back management variable
extern mega_systemTimerCall systemTimerCall;


typedef struct  __attribute__((__packed__)) {
	volatile uint16_t stepCounterB; //step counter backwards
	volatile int8_t stepDrift;  //steps exceeding the command steps
	volatile uint16_t position; //engine position
	volatile uint16_t ecoderMills;  //milliseconds elapsed between two steps
	//volatile uint16_t deltaPreMillsF; //fraction of milliseconds
	volatile uint8_t forward; //direction calculated from the encoder data
	volatile uint16_t timeLapse;  //time interval = value of TCNT register (1/16000 milliseconds)
	volatile int8_t deltaPosition;  //number of encoder steps between the sampling interval of the system time routine (positive forward, negative back)
	volatile int8_t pause;	//1 command paused 0 immediate command
	volatile uint16_t pauseCount; //pause counter backwards
	volatile int8_t cmdStop;  //there was an engine stop
	volatile int8_t cmdEnd;  //end of the current command
	volatile int8_t cmdRun;   //the current command is running
	volatile int8_t cmdStart; //the current command is present
	volatile int8_t cmdPause; //the current command is paused
} mega_systemTimerFB;

//global feed back management variable
extern mega_systemTimerFB systemTimerFB;
extern mega_systemTimerFB systemTimerExtFB;


extern void inizializesystemTimerFB();
extern void inizializeSerial();

#define CMDEXT_SETSTOP systemTimerExtFB.cmdStop = 1;  systemTimerExtFB.cmdRun = 0; systemTimerExtFB.cmdStart = 0; systemTimerExtFB.cmdPause = 0; systemTimerExtFB.cmdEnd = 1

#define CMDEXT_SETRUN systemTimerExtFB.cmdStop = 0;  systemTimerExtFB.cmdRun = 1; systemTimerExtFB.cmdStart = 0; systemTimerExtFB.cmdPause = 0; systemTimerExtFB.cmdEnd = 0

#define CMDEXT_SETSTART systemTimerExtFB.cmdStop = 0;  systemTimerExtFB.cmdRun = 0; systemTimerExtFB.cmdStart = 1; systemTimerExtFB.cmdPause = 0; systemTimerExtFB.cmdEnd = 0

#define CMDEXT_SETEND systemTimerExtFB.cmdStop = 0;  systemTimerExtFB.cmdRun = 0; systemTimerExtFB.cmdStart = 0; systemTimerExtFB.cmdPause = 0; systemTimerExtFB.cmdEnd = 1

#define CMDEXT_SETPAUSE systemTimerExtFB.cmdStop = 0;  systemTimerExtFB.cmdRun = 0; systemTimerExtFB.cmdStart = 0; systemTimerExtFB.cmdPause = 1; systemTimerExtFB.cmdEnd = 0



#define CMD_SETSTOP systemTimerFB.cmdStop = 1;  systemTimerFB.cmdRun = 0; systemTimerFB.cmdStart = 0; systemTimerFB.cmdPause = 0; systemTimerFB.cmdEnd = 1

#define CMD_SETRUN systemTimerFB.cmdStop = 0;  systemTimerFB.cmdRun = 1; systemTimerFB.cmdStart = 0; systemTimerFB.cmdPause = 0; systemTimerFB.cmdEnd = 0

#define CMD_SETSTART systemTimerFB.cmdStop = 0;  systemTimerFB.cmdRun = 0; systemTimerFB.cmdStart = 1; systemTimerFB.cmdPause = 0; systemTimerFB.cmdEnd = 0

#define CMD_SETEND systemTimerFB.cmdStop = 0;  systemTimerFB.cmdRun = 0; systemTimerFB.cmdStart = 0; systemTimerFB.cmdPause = 0; systemTimerFB.cmdEnd = 1

#define CMD_SETPAUSE systemTimerFB.cmdStop = 0;  systemTimerFB.cmdRun = 0; systemTimerFB.cmdStart = 0; systemTimerFB.cmdPause = 1; systemTimerFB.cmdEnd = 0

/////////////////////////////////////////////////////////////////////////////////
//data structure used in the system command reset message

typedef struct  __attribute__((__packed__)) {
	uint8_t commandReset;
	uint8_t reset;
  uint8_t restoreMove;
} mega_systemReset;

extern mega_systemReset systemReset;

#endif
