#include "feedback.h"

FB_feedBackData feedBackData;

void initializeFeedBackData() {
	feedBackData.speed = 0;   //calculated speed
	feedBackData.speedTarget = 0; //speed Target
	feedBackData.nStepLeft = 0; //number of missing encoder steps
	feedBackData.driftNstep = 0;  //steps exceeding the command steps
	feedBackData.forward = 1; //direction of rotation of the motor 1 = increase of the steps 0 = decrease of the steps
	feedBackData.deltaPosition = 0; //number of steps in the time interval
	feedBackData.LastPosition = 0;  //previous position
	feedBackData.ecoderMills = 0; //milliseconds elapsed in the time interval
	feedBackData.deltaMillsF = 0; //fraction of milliseconds elapsed in the time interval
	feedBackData.oldEnablePid = 0;   //previous enablePid

	feedBackData.pTerm = 0; //term proportion of pid control
	feedBackData.iTerm = 0; //integral term of the pid control
	feedBackData.dTerm = 0; //derivative term of the pid control
}
