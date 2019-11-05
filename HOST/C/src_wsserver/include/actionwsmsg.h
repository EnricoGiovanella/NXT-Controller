#ifndef ACTIONWSMSG_H
#define ACTIONWSMSG_H

//--------------------------- action on WS SERIALOPEN message ---------------------------

extern void wsActionSerialOpen();

//--------------------------- action on WS SERIALCLOSE message --------------------------

extern void wsActionSerialClose();


//--------------------------- action on WS MOVE message --------------------------

extern void wsActionMove();

//--------------------------- action on WS PARAM2AVR message --------------------------

extern void wsActionParam2avr();

//--------------------------- action on WS DIRECTMOVE message --------------------------

extern void wsActionDirectMove();

//--------------------------- action on WS SYSTEMCMD message --------------------------
extern void printSystemCmdCmd(int tipo);

extern void wsActionSystemCmd();


#endif
