#include <stdio.h>
#include <stdint.h>
#include "include/tools.h"
#include "include/variables.h"

uint8_t hostEndian;
uint8_t convWsEndians = 0;


#define HOSTBIGENDIAN 0
#define HOSTLITTEENDIAN 1

// test to understand host notation
// the web socket client transmits and receives big endians in notation
void testHostEndians() {
	unsigned int x = 1;
	hostEndian = (uint8_t) ((char *)&x)[0];
	// no conversion to transmit and receive web socket messages
	if(hostEndian == HOSTBIGENDIAN) {
		fprintf(stderr, "host is big endian\n");
		convWsEndians = 0;
	} else {
		// conversion to transmit and receive web socket messages
		fprintf(stderr, "host is litte endian\n");
		convWsEndians = 1;
	}
}

