#ifndef ATMEGA2560DATA_H
#define ATMEGA2560DATA_H

#include <stdint.h>
#include "message.h"

extern uint8_t commandReset;

/////////////////////////////////////////////////////////////////////////////////
//data structure used by the encoder interrupt routine and the pwm timer


typedef struct  __attribute__((__packed__)) {
	volatile uint8_t oldEncoderState;	//previous state of the encoder inputs
	volatile uint8_t enableStopPosition;	//enables motor stop if command provides (1)
	volatile uint8_t enableEndPosition;	//enables the control of the end of the command that is when moveCommandData.nStepTarget through endPosition set by the system timer routine (1)
	volatile int8_t endPosition;	//steps missing at the end of the command
	volatile uint8_t cmdEnd;	//raggiunta la fine del commando
	volatile int8_t deltaPosition;	//number of encoder steps between the sampling interval of the system time routine (positive forward, negative back)
	volatile uint16_t timeLapse;	//time interval = value of TCNT register (1/16000 milliseconds)
//variables used by bridge H
	volatile uint16_t pwmHbrigde;	//pwm value of bridge H
	volatile uint8_t engineStop;	//flag 1 = the motor is stopped
	volatile uint8_t engineForward;	//flag 1 = the motor turns forward
	volatile uint8_t engineBack;	//flag 1 = the engine turns bac
} mega_encoder;

extern mega_encoder encoderData;

/////////////////////////////////////////////////////////////////////////////////
//data structure used for transmission and in the system timer interrupt routine


#define MAX_BUFFRX 40
#define MAX_BUFFTX 40
#define RECEIVE_DELAY_TIME 2

typedef struct  __attribute__((__packed__)) {
	volatile uint8_t buffRX[MAX_BUFFRX];
	volatile uint8_t countRX;     // receive byte counter
	volatile uint8_t codMsgRX;    // 0 = no message
	volatile uint8_t codMsgDetectRX; //flag detect receive message
	volatile uint8_t delayCountRX;  // delay respose message
	volatile uint8_t msgErrorRX;    //flag error receive
	volatile uint8_t UCSRA;		//status register of the UART
	volatile uint8_t replyMsgRunRX; //flag run function replyMsg()

	volatile uint8_t TXBuff[MAX_BUFFTX]; // transmission buffer
	volatile uint8_t TXCount; // TX buffer counter
	uint8_t TXEnable; // enable transmission used in 
	volatile uint8_t TXMsgReady; // message ready
	volatile uint8_t TXCodMsg; // message code to be transmitted
	volatile uint8_t TXMsgSysReady; // message system ready
	volatile uint8_t TXSysCodMsg; // message ystem code to be transmitted
	volatile uint8_t TXLenght; // message length
	volatile uint8_t TXBusy;

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
	volatile uint8_t moveCommand1Present;  //indicates the presence of a command in moveCommand1
	volatile uint8_t moveCommand2Present;  //indicates the presence of a command in moveCommand2
	volatile uint8_t currentMoveCommand;  //indicates which structure is used as the current command
	volatile uint8_t paramChange;	//the parameters received have changed
} mega_systemTimerCall;

//global feed back management variable
extern mega_systemTimerCall systemTimerCall;


typedef struct  __attribute__((__packed__)) {
	volatile msgR_move *commandPointer;	//pointer to the current command
	volatile uint16_t stepCounterB;	//step counter backwards
	volatile int8_t stepDrift;	//steps exceeding the command steps
	volatile uint16_t position;	//engine position
	volatile uint16_t ecoderMills;	//milliseconds elapsed between two steps
	//volatile uint16_t deltaPreMillsF; //fraction of milliseconds
	volatile uint8_t forward;	//direction calculated from the encoder data
	volatile uint16_t timeLapse;	//time interval = value of TCNT register (1/16000 milliseconds)
	volatile int8_t deltaPosition;	//number of encoder steps between the sampling interval of the system time routine (positive forward, negative back)
	volatile int8_t cmdStop;	//there was an engine stop
	volatile int8_t cmdRun;		//the current command is running
	volatile int8_t cmdStart;	//the current command has been acquired
	volatile int8_t cmdPause;	//the current command is paused
	volatile int8_t nextCmd;	//2 indicates that the next command exists
} mega_systemTimerFB;

//global feed back management variable
extern mega_systemTimerFB systemTimerFB;
extern mega_systemTimerFB systemTimerExtFB;

 
extern void inizializesystemTimerFB();

#endif
