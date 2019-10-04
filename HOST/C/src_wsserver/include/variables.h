#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdint.h>
#include "messagews.h"

extern int active;
// if active == 0  threadRs232 terminate

extern int users;
// number of users connected with the web socket

extern uint8_t convWsEndians;
//if 1 conversion endians needed on the web socket channel

// ----------------------- instances of web socket message structures -----------------------

extern WS_serialOpen wsSerialOpen;
extern WS_serialStates wsSerialStates;

#endif

