#ifndef MESSAGE_INLINE_H
#define MESSAGE_INLINE_H

#include "message.h"
#include "atmega2560data.h"
#include "feedback.h"

inline void setCommandStatus() {
  msgTcommandStatus.speed = feedBackData.speed;
  msgTcommandStatus.position = systemTimerExtFB.position;
  msgTcommandStatus.nStepLeft = systemTimerExtFB.stepCounterB;
  msgTcommandStatus.driftNstep = systemTimerExtFB.stepDrift;
  msgTcommandStatus.pwmHbridge = encoderData.pwmHbrigde;
  msgTcommandStatus.forward = systemTimerExtFB.forward;
  msgTcommandStatus.pTerm = feedBackData.pTerm;
  msgTcommandStatus.iTerm = feedBackData.iTerm;
  msgTcommandStatus.dTerm = feedBackData.dTerm;
  if(systemTimerExtFB.cmdPause) msgTcommandStatus.status |= STATUS_PAUSE; else msgTcommandStatus.status &= ~STATUS_PAUSE;
  if(!systemTimerExtFB.cmdStart)
    msgTcommandStatus.status |= STATUS_PRESENT; else msgTcommandStatus.status &= ~STATUS_PRESENT;
  if(systemTimerExtFB.cmdRun) msgTcommandStatus.status |= STATUS_RUN; else msgTcommandStatus.status &= ~STATUS_RUN;
  if(systemTimerExtFB.cmdStop) msgTcommandStatus.status |= STATUS_STOP; else msgTcommandStatus.status &= ~STATUS_STOP;
  if(systemTimerExtFB.nextCmd == 2) msgTcommandStatus.status |= STATUS_NEXT; else msgTcommandStatus.status &= ~STATUS_NEXT;
  if(feedBackData.reset) msgTcommandStatus.status |= STATUS_RESET; else msgTcommandStatus.status &= ~STATUS_RESET;
  feedBackData.reset = 0;
}

inline void readEEPROMdefaulData() { 
  eeprom_read_block((void*)&msgTdefaultData,(const void*)&EEPROMdefaulData,DIMT_DEFAULTDATA);
}
//void eeprom_read_block (void *__dst, const void *__src, size_t __n);

inline void writeEEPROMdefaulData() {
  eeprom_write_block((const void*)&msgTdefaultData,(void*)&EEPROMdefaulData,DIMT_DEFAULTDATA);
}
//void eeprom_write_block (const void *__src, void *__dst, size_t __n);

// transmission functions

inline int loadTxMsg(uint8_t codMsg) {
  uint8_t *p;
  serial.TXLenght = 0;
  switch (codMsg) {
    case MSGT_SYSTEMERROR:
      msgTsystemError.UCSRA = serial.UCSRA;
      p = (uint8_t *) &msgTsystemError;
      for (uint8_t i = 0; i < DIMT_SYSTEMERROR; i++) serial.TXBuff[i] = p[i];
      serial.TXLenght = DIMT_SYSTEMERROR;
      return codMsg;
    case MSGT_SYSTEMOK:
      msgTsystemOk.msgCodRX = serial.codMsgRX;
      p = (uint8_t *) &msgTsystemOk;
      for (uint8_t i = 0; i < DIMT_SYSTEMOK; i++) serial.TXBuff[i] = p[i];
      serial.TXLenght = DIMT_SYSTEMOK;
      return codMsg;
    case MSGT_SYSTEMTEST:
      p = (uint8_t *) &msgTsystemTest;
      for (uint8_t i = 0; i < DIMT_SYSTEMTEST; i++) serial.TXBuff[i] = p[i];
      serial.TXLenght = DIMT_SYSTEMTEST;
      return codMsg;
    case MSGT_COMMANDSTATUS:
      p = (uint8_t *) &msgTcommandStatus;
      for (uint8_t i = 0; i < DIMT_COMMANDSTATUS; i++) serial.TXBuff[i] = p[i];
      serial.TXLenght = DIMT_COMMANDSTATUS;
      return codMsg;
    case MSGT_DEFAULTDATA:
      p = (uint8_t *) &msgTdefaultData;
      for (uint8_t i = 0; i < DIMT_DEFAULTDATA; i++) serial.TXBuff[i] = p[i];
      serial.TXLenght = DIMT_DEFAULTDATA;
      return codMsg;
  }
  return 0;
}


//reception functions

// msgRsystemCmd.cmd
// 0 = enable transmission, 1 = test request endians, 2 = request for default parameters,
// 3 = activate command paused, 4 write EEPROM 5 suspend transmission 6 reset cmd
inline uint8_t processSystemCmd() {
  if (serial.countRX != (DIMR_SYSTEMCMD + 1)) return 0;
  uint8_t *dest = 0;
  dest = (uint8_t *) &msgRsystemCmd;
  for (uint8_t i = 1; i < serial.countRX; i++) dest[i - 1] = serial.buffRX[i];
  switch (msgRsystemCmd.cmd) {
    case 0:
      serial.TXEnable = 1; //used in inline void feedBack () read only
      return 1; //response MSGT_SYSTEMOK
    case 1:
      serial.TXMsgReady = 1;
      serial.TXCodMsg = MSGT_SYSTEMTEST;
      return 2; // no response MSGT_SYSTEMOK
    case 2:
      serial.TXMsgReady = 1;
      serial.TXCodMsg = MSGT_DEFAULTDATA;
      return 2;
    case 3: 
      systemTimerFB.commandPointer->pause = 0;
      return 1; // response MSGT_SYSTEMOK
    case 4:
      //writeEEPROMdefaulData();
      return 1;
    case 5:
      serial.TXEnable = 0;
      return 1;
    case 6:
      commandReset = 1;
      //serial.TXMsgReady = 1;
      //serial.TXCodMsg = MSGT_DEFAULTDATA;
      return 2;
  }
  return 0; // response MSGT_SYSTEMERROR
}

inline uint8_t processMove() {
  if (serial.countRX != (DIMR_MOVE + 1)) return 0;
  uint8_t *dest = NULL;
  switch(systemTimerCall.currentMoveCommand) {
    case 0:
      if(!systemTimerCall.moveCommand1Present) {
        dest =(uint8_t *) &moveCommand1Data;
        for (uint8_t i = 1; i < serial.countRX; i++) dest[i - 1] = serial.buffRX[i];
        return 1; //response MSGT_SYSTEMOK
      } else {
        if(!systemTimerCall.moveCommand2Present) {
          dest =(uint8_t *)  &moveCommand2Data;
          for (uint8_t i = 1; i < serial.countRX; i++) dest[i - 1] = serial.buffRX[i];
          return 1; //response MSGT_SYSTEMOK
        }
      }
    break;
    case 1:
      if(!systemTimerCall.moveCommand2Present) {
        dest =(uint8_t *)  &moveCommand2Data;
        for (uint8_t i = 1; i < serial.countRX; i++) dest[i - 1] = serial.buffRX[i];
        return 1; //response MSGT_SYSTEMOK
      }
    break;
    case 2:
      if(!systemTimerCall.moveCommand1Present) {
        dest =(uint8_t *)  &moveCommand1Data;
        for (uint8_t i = 1; i < serial.countRX; i++) dest[i - 1] = serial.buffRX[i];
        return 1; //response MSGT_SYSTEMOK
      }
    break;
  }
  return 0; // response MSGT_SYSTEMERROR
}

inline uint8_t processPidControl() {
  if (serial.countRX != (DIMR_MOVE + 1)) return 0;
  uint8_t *dest = (uint8_t *) &pidData;
  for (uint8_t i = 1; i < serial.countRX; i++) dest[i-1] = serial.buffRX[i];

  msgTdefaultData.engineStartThreshold = pidData.engineStartThreshold;  //engine starting threshold
  msgTdefaultData.enablePID = pidData.enablePID;  //enable PID control
  msgTdefaultData.Kp = pidData.Kp;  //proportional pid coefficient
  msgTdefaultData.Ki = pidData.Ki;  //integral pid coefficient
  msgTdefaultData.Kd = pidData.Kd;  //pid derivative coefficient
  msgTdefaultData.FBSampleTime = pidData.FBSampleTime; //sampling interval of the feed back function
  msgTdefaultData.ImaxSum = pidData.ImaxSum;  //maximum value of the integral component of the pid control
  msgTdefaultData.IminSum = pidData.IminSum;  //minimum value of the integral component of the pid control
  msgTdefaultData.maxPWM = pidData.maxPWM;    //maximum value of pid control output
  msgTdefaultData.minPWM = pidData.minPWM;    //minimum value of pid control output

  systemTimerCall.paramChange = 1;
  return 1;
}

inline uint8_t processMsg(uint8_t codMsg) {
  switch (codMsg) {
    case MSGR_SYSTEMCMD:
      return processSystemCmd();
    case MSGR_MOVE:
      return processMove();
    /*case MSGR_DIRECTFB:
      return processDirectFB();*/
    case MSGR_PIDCONTROL:
      return processPidControl();
  }
  return 0;
}


#endif
