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
	msgTcommandStatus.forward = encoderData.engineForward;
	msgTcommandStatus.pTerm = feedBackData.pTerm;
	msgTcommandStatus.iTerm = feedBackData.iTerm;
	msgTcommandStatus.dTerm = feedBackData.dTerm;
	if (systemTimerExtFB.cmdPause) msgTcommandStatus.status |= STATUS_PAUSE; else msgTcommandStatus.status &= ~STATUS_PAUSE;
	if (systemTimerExtFB.cmdStart) msgTcommandStatus.status |= STATUS_START; else msgTcommandStatus.status &= ~STATUS_START;
	if (systemTimerCall.moveCommandPresent) msgTcommandStatus.status |= STATUS_PRESENT; else msgTcommandStatus.status &= ~STATUS_PRESENT;
	if (systemTimerExtFB.cmdRun) msgTcommandStatus.status |= STATUS_RUN; else msgTcommandStatus.status &= ~STATUS_RUN;
	if (systemTimerExtFB.cmdStop) {
		msgTcommandStatus.status |= STATUS_STOP;
		msgTcommandStatus.forward = 2;
	} else msgTcommandStatus.status &= ~STATUS_STOP;
	if (systemTimerExtFB.cmdEnd) msgTcommandStatus.status |= STATUS_END; else msgTcommandStatus.status &= ~STATUS_END;
	if (systemReset.reset) msgTcommandStatus.status |= STATUS_RESET; else msgTcommandStatus.status &= ~STATUS_RESET;
}

inline void readEEPROMdefaulData() {
	eeprom_read_block((void*)&msgTdefaultData, (const void*)&EEPROMdefaulData, DIMT_DEFAULTDATA);
}
//void eeprom_read_block (void *__dst, const void *__src, size_t __n);

inline void writeEEPROMdefaulData() {
	eeprom_write_block((const void*)&msgTdefaultData, (void*)&EEPROMdefaulData, DIMT_DEFAULTDATA);
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
			return;
		case MSGT_SYSTEMOK:
			p = (uint8_t *) &msgTsystemOk;
			for (uint8_t i = 0; i < DIMT_SYSTEMOK; i++) serial.TXBuff[i] = p[i];
			serial.TXLenght = DIMT_SYSTEMOK;
			return;
		case MSGT_COMMANDSTATUS:
			p = (uint8_t *) &msgTcommandStatus;
			for (uint8_t i = 0; i < DIMT_COMMANDSTATUS; i++) serial.TXBuff[i] = p[i];
			serial.TXLenght = DIMT_COMMANDSTATUS;
			return;
		case MSGT_DEFAULTDATA:
			p = (uint8_t *) &msgTdefaultData;
			for (uint8_t i = 0; i < DIMT_DEFAULTDATA; i++) serial.TXBuff[i] = p[i];
			serial.TXLenght = DIMT_DEFAULTDATA;
			return;
	}
}


//reception functions

// msgRsystemCmd.cmd
// 0 = enable transmission, 1 = test request endians, 2 = request for default parameters,
// 3 = activate command paused, 4 write EEPROM 5 suspend transmission 6 reset cmd 7 set directCmd = 1
// 8 directCmd = 0
inline uint8_t processSystemCmd() {
	uint8_t *dest = 0;
	dest = (uint8_t *) &msgRsystemCmd;
	for (uint8_t i = 0; i < DIMR_SYSTEMCMD; i++) dest[i] = serial.buffRX[i];
	switch (msgRsystemCmd.cmd) {
		case 0:	// 0 = enable transmission
			serial.TXEnable = 1; //used in inline void trasmitCmdStatus() read only
			serial.TXCodMsg = MSGT_COMMANDSTATUS;
			return; //response MSGT_SYSTEMOK
		case 1:	// 1 = communication test
			msgTsystemOk.subMsgCodRX = 1;
			msgTsystemOk.msgCodRX = MSGR_SYSTEMCMD;
			serial.TXCodMsg = MSGT_SYSTEMOK;
			return;
		case 2:	//2 = request for default parameters
			msgTsystemOk.subMsgCodRX = 2;
			//readEEPROMdefaulData();
			serial.TXCodMsg = MSGT_DEFAULTDATA;
			return;
		case 4:
			//writeEEPROMdefaulData();
			msgTsystemOk.subMsgCodRX = 4;
			serial.TXCodMsg = MSGT_DEFAULTDATA;
			return;
		case 5: //5 = suspend transmission
			serial.TXEnable = 0;
			msgTsystemOk.subMsgCodRX = 5;
			msgTsystemOk.msgCodRX = MSGR_SYSTEMCMD;
			serial.TXCodMsg = MSGT_SYSTEMOK;
			return;
		case 6:	//6 = reset cmd and set direct cmd
			systemReset.commandReset = 1;
			msgTdefaultData.directCmd = 1;
			msgTsystemOk.subMsgCodRX = 6;
			serial.TXCodMsg = MSGT_DEFAULTDATA;
			return;
		case 7:	//7 = set directCmd
			systemReset.commandReset = 1;
			msgTsystemOk.subMsgCodRX = 7;
			serial.TXCodMsg = MSGT_DEFAULTDATA;
			return;
		case 8:	//8 = reset directCmd
			uint8_t oldDirectCmd = msgTdefaultData.directCmd;
			if (oldDirectCmd) systemReset.restoreMove = 1;
			msgTsystemOk.subMsgCodRX = 8;
			serial.TXCodMsg = MSGT_DEFAULTDATA;
			return;
	}
	serial.TXCodMsg = MSGT_SYSTEMERROR;  // response MSGT_SYSTEMERROR
}

inline uint8_t processMove() {
	uint8_t* dest = (uint8_t *) &moveCommandData;
	for (uint8_t i = 0; i < DIMR_MOVE; i++) dest[i] = serial.buffRX[i];
	systemTimerCall.moveCommandPresent = 1;
	uint8_t oldDirectCmd = msgTdefaultData.directCmd;
	if (oldDirectCmd) systemReset.restoreMove = 1;
	serial.TXCodMsg = MSGT_COMMANDSTATUS;
}

inline uint8_t processPidControl() {
	uint8_t oldDirectCmd = msgTdefaultData.directCmd;
	uint8_t* dest = (uint8_t *) &msgTdefaultData;
	for (uint8_t i = 0; i < DIMR_PIDCONTROL; i++) dest[i] = serial.buffRX[i];
	msgTdefaultData.fifo = 0;
	if (oldDirectCmd) systemReset.restoreMove = 1;
	serial.TXCodMsg = MSGT_DEFAULTDATA;
}


inline uint8_t processDirectMove() {
	uint8_t *dest = (uint8_t *) &directMove;
	for (uint8_t i = 0; i < DIMR_DIRECTMOVE; i++) dest[i] = serial.buffRX[i];
	systemReset.commandReset = 1;
	msgTdefaultData.directCmd = 1;
	serial.TXCodMsg = MSGT_COMMANDSTATUS;
}


inline uint8_t processMsg(uint8_t codMsg) {
	switch (codMsg) {
		case MSGR_SYSTEMCMD:
			processSystemCmd();
			return;
		case MSGR_MOVE:
			processMove();
			return;
		case MSGR_PIDCONTROL:
			processPidControl();
			return;
		case MSGR_DIRECTMOVE:
			processDirectMove();
			return;
	}
	return 0;
}


#endif
