#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <avr/eeprom.h>

//messages to be transmitted

typedef struct  __attribute__((__packed__)) {
	uint8_t UCSRA;
} msgT_systemError;

extern msgT_systemError msgTsystemError;

#define MSGT_SYSTEMERROR 23

#define DIMT_SYSTEMERROR sizeof(msgT_systemError)

typedef struct  __attribute__((__packed__)) {
	uint8_t msgCodRX;
	uint16_t subMsgCodRX;
	uint16_t subMsgCodRX1;
} msgT_systemOk;

extern msgT_systemOk msgTsystemOk;

#define MSGT_SYSTEMOK 33

#define DIMT_SYSTEMOK sizeof(msgT_systemOk)


#define STATUS_PAUSE (0x01)
#define STATUS_PRESENT (0x02)
#define STATUS_RUN (0x04)
#define STATUS_STOP (0x08)
#define STATUS_RESET (0x10)
#define STATUS_END (0x20)
#define STATUS_START (0x40)

/*
    bit 0 = 1 command paused 0
    bit 1 = 1 command present 0
    bit 2 = 1 command running 0 executed
    bit 3 = 1 stop executed 0 stop to execute
    ////bit 4 = 1 next command present 0 next command absent NO
    bit 4 = 1 Reset Cmd (engine stop)
    bit 5 = 1 command executed
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
	uint8_t codCmd;   //message numeric code
} msgT_commandStatus;

extern msgT_commandStatus msgTcommandStatus;

#define MSGT_COMMANDSTATUS 43

#define DIMT_COMMANDSTATUS sizeof(msgT_commandStatus)


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
	volatile uint8_t directCmd;  //direct command to the motor by the host
	uint8_t fifo; //1 fifo 0 direct
} msgT_defaultData;

extern msgT_defaultData msgTdefaultData;

#define MSGT_DEFAULTDATA 53

#define DIMT_DEFAULTDATA sizeof(msgT_defaultData)

extern uint8_t EEMEM EEPROMdefaulData[DIMT_DEFAULTDATA];

//messages to receive

typedef struct  __attribute__((__packed__)) {
	uint8_t cmd;
} msgR_systemCmd;

// 0 = enable transmission, 1 = test request endians, 2 = request for default parameters,
// 3 = not used, 4 = write EEPROM 5  =suspend transmission 6 = reset cmd and set direct cmd
// 7 = set directCmd 8 = reset directCmd

extern msgR_systemCmd msgRsystemCmd;

#define MSGR_SYSTEMCMD 13

#define DIMR_SYSTEMCMD sizeof(msgR_systemCmd)


typedef struct  __attribute__((__packed__)) {
	uint16_t speedTarget;  //target motor speed (0 1750)
	uint16_t nStepTarget;  //number of encoder steps (max 65535 steps)
	uint8_t forward; //direction of rotation of the motor 1 = increase of the steps 0 = decrease of the steps
	uint8_t enableStop;  //stops the motor at the end of the command by decreasing from nStepEnd
	uint8_t pause; //1 command paused 0 immediate command
	uint16_t millsPause; //pause millis
	uint8_t fifo; //1 fifo 0 direct
	uint8_t codCmd;
} msgR_move;

extern msgR_move moveCommandData;

//extern msgR_move moveCommand2Data;

#define MSGR_MOVE 73

#define DIMR_MOVE sizeof(msgR_move)

#define MSGR_PIDCONTROL 63

#define DIMR_PIDCONTROL sizeof(msgT_defaultData)

typedef struct  __attribute__((__packed__)) {
	uint16_t speedTarget;  //target motor speed (0 1750)
	uint8_t forward; //direction of rotation of the motor 1 = increase of the steps 0 = decrease of the steps
} msgR_directMove;

extern msgR_directMove directMove;

#define MSGR_DIRECTMOVE 83

#define DIMR_DIRECTMOVE sizeof(msgR_directMove)

extern void initializeMessage();

#endif
