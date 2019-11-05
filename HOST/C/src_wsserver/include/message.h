#ifndef MESSAGEWS_H
#define MESSAGEWS_H

#include <stdint.h>

//------------------------------ WS SERIALOPEN message data structure ------------------------------
// receive WS

#define WSMSG_SERIALOPEN 3

typedef struct  __attribute__((__packed__)) {
	char device[20];
	uint32_t baud;
	uint8_t parity;
	uint8_t stop;
	uint8_t data;
} WS_serialOpen;

extern WS_serialOpen wsSerialOpen;

#define VAR_SERIALOPEN WS_serialOpen wsSerialOpen

#define DIM_SERIALOPEN sizeof(WS_serialOpen)

//----------------------------- messagge WS SERIALCLOSE no data ------------------------------------
// receive WS

#define WSMSG_SERIALCLOSE 4

//----------------------------- WS SERIALSTATES message data structure -----------------------------
// trasmit WS

#define WSMSG_SERIALSTATES 5

typedef struct  __attribute__((__packed__)) {
	uint8_t states;
	uint8_t codE;
} WS_serialStates;

extern WS_serialStates wsSerialStates;

#define VAR_SERIALSTATES WS_serialStates wsSerialStates

#define DIM_SERIALSTATES sizeof(WS_serialStates)





//------------------------------ WS COMMANDSTATUS message data structure ------------------------------
// trasmit WS
// receive AVR

#define WSMSG_COMMANDSTATUS 6
#define AVRR_COMMANDSTATUS 43

/*
    bit 0 = 1 comando in pausa 0 comando immediato
    bit 1 = 1 comando presente 0 comando assente
    bit 2 = 1 comando in esecuzione 0 eseguito
    bit 3 = 1 stop eseguito 0 stop da eseguire
    bit 4 = 1 comando successivo presente 0 comando successivo assente
*/

/*
    bit 0 = 1 command paused 0 immediate command
    bit 1 = 1 command present 0 command absent
    bit 2 = 1 command running 0 executed
    bit 3 = 1 stop executed 0 stop to execute
    bit 4 = 1 next command present 0 next command absent
*/


typedef struct  __attribute__((__packed__)) {
	uint16_t speed;   //real engine speed
	uint16_t position;  //engine position
	uint16_t nStepLeft; //number of missing encoder steps
	int8_t driftNstep;  //steps exceeding the command steps
	uint16_t pwmHbridge;  //pwm input value of bridge H
	uint8_t forward;  //direction of rotation of the motor 1 = increase of the steps 0 = decrease of the steps
	int16_t pTerm;    //proportional part of the pid control
	int16_t iTerm;    //integral part of the pid control
	int16_t dTerm;    //derived part of the pid control
	uint8_t status;   //status as described above
	uint8_t directCmd; //direct command to the motor by the host
	uint8_t codCmd; 	//message numeric code
} WS_commandStatus;


extern WS_commandStatus wsCommandStatus;
//extern WS_commandStatus avrRcommandStatus;

#define VAR_COMMANDSTATUS WS_commandStatus wsCommandStatus
#define AVR_COMMANDSTATUS WS_commandStatus avrRcommandStatus

#define DIM_COMMANDSTATUS sizeof(WS_commandStatus)

//------------------------------ WS PARAMAVR PARAM2AVR message data structure ------------------------------
// trasmit by AVR AVRR_DEFAULTDATA --> wsserver --> sent to WS WSMSG_PARAMAVR

// trasmit by web interface WSMSG_PARAM2AVR --> wsserver --> sent to AVR AVRT_PIDCONTROL

#define WSMSG_PARAMAVR 7
#define AVRR_DEFAULTDATA 53

#define WSMSG_PARAM2AVR 9
#define AVRT_PIDCONTROL 63

typedef struct  __attribute__((__packed__)) {
	uint8_t enablePID;  //enable PID control
	float Kp; //proportional pid coefficient
	float Ki; //integral pid coefficient
	float Kd; //pid derivative coefficient
	uint16_t FBSampleTime;  //sampling interval of the feed back function
	int16_t ImaxSum;  //maximum value of the integral component of the pid control
	int16_t IminSum;  //minimum value of the integral component of the pid control
	int16_t maxPWM;   //maximum value of pid control output
	int16_t minPWM;   //minimum value of pid control output
	uint16_t engineStartThreshold;  //engine starting threshold
	uint8_t directCmd;  //direct command to the motor by the host
	uint8_t fifo;	//1 fifo 0 direct
} WS_paramAvr;

extern WS_paramAvr wsParamAvr;
extern WS_paramAvr wsParam2avr;
//extern WS_paramAvr avrRdefaultData;

#define VAR_PARAMAVR WS_paramAvr wsParamAvr
#define VAR_PARAM2AVR WS_paramAvr wsParam2avr
#define AVR_DEFAULTDATA WS_paramAvr avrRdefaultData

#define DIM_PIDCONTROL sizeof(WS_paramAvr)
#define DIM_DEFAULTDATA sizeof(WS_paramAvr)

//------------------------------ WS MOVE message data structure ------------------------------
// receive WS
// trasmit AVR

#define WSMSG_MOVE 8
#define AVRT_MOVE 73

typedef struct  __attribute__((__packed__)) {
	uint16_t speedTarget;  //target motor speed (0 1750)
	uint16_t nStepTarget;  //number of encoder steps (max 65535 steps)
	uint8_t forward; //direction of rotation of the motor 1 = increase of the steps 0 = decrease of the steps
	uint8_t enableStop;  //stops the motor at the end of the command by decreasing from nStepEnd
	uint8_t pause; //1 command paused 0 immediate command
	uint16_t millsPause; //pause millis
	uint8_t fifo;	//1 fifo 0 direct
	uint8_t codCmd; 
} WS_move;

extern WS_move wsMove;

#define VAR_MOVE WS_move wsMove

#define DIM_MOVE sizeof(WS_move)

//------------------------------ WS DIRECTMOVE message data structure ------------------------------
// receive

#define WSMSG_DIRECTMOVE 10
#define AVRT_DIRECTMOVE 83


typedef struct  __attribute__((__packed__)) {
	uint16_t speedTarget;  //target motor speed (0 1750)
	uint8_t forward; //direction of rotation of the motor 1 = increase of the steps 0 = decrease of the steps
} WS_directMove;

extern WS_directMove wsDirectMove;

#define VAR_DIRECTMOVE WS_directMove wsDirectMove

#define DIM_DIRECTMOVE sizeof(WS_directMove)

//------------------------------ WS SYSTEMCMD message data structure ------------------------------
// receive WS
// trasmit AVR

#define WSMSG_SYSTEMCMD 11
#define AVRT_SYSTEMCMD 13

typedef struct  __attribute__((__packed__)) {
	uint8_t cmd;
} WS_systemCmd;

// 0 = enable transmission, 1 = test request endians, 2 = request for default parameters,
// 3 = not used, 4 = write EEPROM 5  =suspend transmission 6 = reset cmd and set direct cmd
// 7 = set directCmd 8 = reset directCmd

extern WS_systemCmd wsSystemCmd;

#define VAR_SYSTEMCMD WS_systemCmd wsSystemCmd

#define DIM_SYSTEMCMD sizeof(WS_systemCmd)


//------------------------------ AVR SYSTEMERROR message data structure ----------------------------
// receive AVR

#define AVRR_SYSTEMERROR 23

typedef struct  __attribute__((__packed__)) {
  uint8_t UCSRA;
}  avrR_systemError;

extern avrR_systemError avrRsystemError;

#define VAR_SYSTEMERROR avrR_systemError avrRsystemError

#define DIM_SYSTEMERROR sizeof(avrR_systemError)

//------------------------------ AVR SYSTEMOK message data structure -------------------------------
// receive AVR

#define AVRR_SYSTEMOK 33

typedef struct  __attribute__((__packed__)) {
  uint8_t msgCodRX;
  uint16_t subMsgCodRX;
  uint16_t subMsgCodRX1;
} avrR_systemOk;

extern avrR_systemOk avrRsystemOk;

#define DIM_SYSTEMOK sizeof(avrR_systemOk)

#define STATUS_PAUSE (0x01)
#define STATUS_PRESENT (0x02)
#define STATUS_RUN (0x04)
#define STATUS_STOP (0x08)
#define STATUS_RESET (0x10)
#define STATUS_END (0x20)
#define STATUS_START (0x40)

//-------------------------- transmit WS SERIALSTATES message --------------------------

extern void transmitMsgWsSerialStates();


//------------------------------ transmit WS COMMANDSTATUS message ---------------------

extern void transmitMsgWsCommandStatus();

//------------------------------ transmit WS PARAMAVR message --------------------------

extern void transmitMsgWsParamAvr();






//----------------------------- receive message WS SERIALOPEN -----------------------------

extern void receiveMsgWsSerialOpen(unsigned char* msg,size_t len);

//----------------------------- receive message WS SERIALCLOSE ----------------------------

extern void receiveMsgWsSerialClose(unsigned char* msg,size_t len);


//------------------------------ receive message WS MOVE ----------------------------------

extern void receiveMsgWsMove(unsigned char* msg,size_t len);

//------------------------------ receive message WS PARAM2AVR -----------------------------

extern void receiveMsgWsParam2Avr(unsigned char* msg,size_t len);

//------------------------------ receive message WS DIRECTMOVE ----------------------------

extern void receiveMsgWsDirectMove(unsigned char* msg,size_t len);

//------------------------------ receive message WS SYSTEMCMD -----------------------------

extern void receiveMsgWsSystemCmd(unsigned char* msg,size_t len);

//------------------------------ WS COMMANDSTATUS message data structure ------------------------------

extern void transmitMsgWsCommandStatus();

//------------------------------ WS PARAMAVR message data structure ------------------------------

extern void transmitMsgWsParamAvr();

//--------- end of message transmission reception specific functions ----------------------


//-------------------------------- main receive function ----------------------------------

extern void receiveBinaryWsMsg(unsigned char* msg,size_t len);




// ----------------------- instances of internal structures ---------------------------------
 
extern WS_paramAvr workParamAvr;



#endif

