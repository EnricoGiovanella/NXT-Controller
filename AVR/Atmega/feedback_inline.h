#ifndef FEEDBACK_INLINE_H
#define FEEDBACK_INLINE_H

#include "feedback.h"

inline void initPID() {
  uint16_t pwmHB = feedBackData.speed;

  if(pwmHB > msgTdefaultData.maxPWM) pwmHB = msgTdefaultData.maxPWM;
  else if(pwmHB < msgTdefaultData.minPWM) pwmHB = msgTdefaultData.minPWM;
  encoderData.pwmHbrigde = pwmHB;

  feedBackData.iTerm = encoderData.pwmHbrigde;
  if(feedBackData.iTerm > msgTdefaultData.ImaxSum)  feedBackData.iTerm = msgTdefaultData.ImaxSum;
  else if(feedBackData.iTerm < msgTdefaultData.IminSum) feedBackData.iTerm = msgTdefaultData.IminSum;
}



inline void pidControl() {
  //error calculation
  float ITerm = (float) feedBackData.iTerm;
  int16_t error = feedBackData.speedTarget - feedBackData.speed;  //max error |1750|
  //integral part contribution
  ITerm += msgTdefaultData.Ki * error;
  if(ITerm > msgTdefaultData.ImaxSum) ITerm = msgTdefaultData.ImaxSum;
  else if(ITerm < msgTdefaultData.IminSum) ITerm = msgTdefaultData.IminSum;
  //derivative
  int16_t dInput = feedBackData.speed - encoderData.pwmHbrigde;

  //proportional part contribution
  float PTerm = msgTdefaultData.Kp * error;

  //derivative part contribution
  float DTerm = - msgTdefaultData.Kd * dInput;

  //calculate output
  uint16_t pwmHbrigde = (uint16_t) PTerm + ITerm +  DTerm;
  if(pwmHbrigde >  msgTdefaultData.maxPWM) pwmHbrigde =  msgTdefaultData.maxPWM;
  else if(pwmHbrigde <  msgTdefaultData.minPWM) pwmHbrigde =  msgTdefaultData.minPWM;
  encoderData.pwmHbrigde = pwmHbrigde;

  //pid data for status message
  feedBackData.pTerm  = (int16_t) ITerm;
  feedBackData.iTerm  = (int16_t) ITerm;
  feedBackData.dTerm  = (int16_t) ITerm;
}


#endif
