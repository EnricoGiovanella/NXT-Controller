#include <stdio.h>
#include "include/usbrs232.h"
#include "include/messagews.h"
#include "include/trasmitwsmsg.h"
#include "include/actionwsmsg.h"
#include "include/variables.h"

//--------------------------- action on WS SERIALOPEN message ---------------------------

void wsActionSerialOpen() {
	fprintf(stderr, "wsSerialOpen.device %s wsSerialOpen.baud %u wsSerialOpen.parity %u wsSerialOpen.stop %u wsSerialOpen.data %u ", wsSerialOpen.device, wsSerialOpen.baud, wsSerialOpen.parity, wsSerialOpen.stop, wsSerialOpen.data);
	setRs232(wsSerialOpen.device, wsSerialOpen.baud, wsSerialOpen.parity, wsSerialOpen.stop, wsSerialOpen.data);
	openRs232();
}

//--------------------------- action on WS SERIALCLOSE message --------------------------

void wsActionSerialClose() {
	fprintf(stderr, "wsSerilaClose");
	closeRs232();
}
