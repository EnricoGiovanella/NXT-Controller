#ifndef MESSAGEWS_H
#define MESSAGEWS_H

#include <stdint.h>

//------------------------------ WS SERIALOPEN message data structure ------------------------------

#define WSMSG_SERIALOPEN 3

typedef struct  __attribute__((__packed__)) {
	char device[20];
	uint32_t baud;
	uint8_t parity;
	uint8_t stop;
	uint8_t data;
} WS_serialOpen;

//extern WS_serialOpen wsSerialOpen

#define VAR_SERIALOPEN WS_serialOpen wsSerialOpen;

//----------------------------- messagge WS SERIALCLOSE no data ------------------------------------

#define WSMSG_SERIALCLOSE 4

//----------------------------- WS SERIALSTATES message data structure -----------------------------

#define WSMSG_SERIALSTATES 5

typedef struct  __attribute__((__packed__)) {
	uint8_t states;
	uint8_t codE;
} WS_serialStates;

//extern WS_serialStates wsSerialStates

#define VAR_SERIALSTATES WS_serialStates wsSerialStates;

#endif
