#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <stdint.h>

typedef struct  __attribute__((__packed__)) {
//variables used to calculate the speed in the feedBack function
	uint16_t speed;		//calculated speed
	uint16_t speedTarget;	//speed Target
	uint16_t nStepLeft;	//number of missing encoder steps
	int16_t driftNstep;	//steps exceeding the command steps
	uint8_t forward;	//direction of rotation of the motor 1 = increase of the steps 0 = decrease of the steps
	volatile uint16_t deltaPosition;	//number of steps in the time interval
	volatile uint16_t LastPosition;	//previous position
	volatile uint16_t ecoderMills;		//milliseconds elapsed in the time interval
	volatile uint16_t deltaMillsF;	//fraction of milliseconds elapsed in the time interval
	uint8_t reset;		//there was a reset
  uint8_t oldEnablePid; //previous enablePid
	
	int16_t pTerm;	//term proportion of pid control
	int16_t iTerm;	//integral term of the pid control
	int16_t dTerm;	//derivative term of the pid control
} FB_feedBackData;

extern void initializeFeedBackData();

extern FB_feedBackData feedBackData;

#endif
