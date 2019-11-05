#ifndef CONFIGURERS232_H
#define CONFIGURERS232_H

#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>

struct conf_serialConfData {
	char device[20];
	speed_t baud;
	uint8_t parity;
	uint8_t stop;
	tcflag_t data;
	uint8_t isOpen;
	//uint8_t close;
	int fd;
} ;

extern struct conf_serialConfData serialConfData;

extern void setRs232(unsigned char* device, uint32_t baud, uint8_t parity, uint8_t stop, uint8_t data);

extern int setInterfaceAttribs(int fd);

#endif
