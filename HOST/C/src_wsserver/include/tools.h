#ifndef TOOLS_H
#define TOOLS_H

#include <stdint.h>

extern void testHostEndians();

// conversion endians type uint32_t
static inline uint32_t convertMsg2U32(unsigned char* in,uint8_t convEndians) {
	if(!convEndians) return (uint32_t) *in;

	unsigned char out[sizeof(uint32_t)];
	for(uint8_t t=0; t<sizeof(uint32_t); t++) out[sizeof(uint32_t)-1-t] = in[t];
	return (uint32_t) *out;
}

#endif
