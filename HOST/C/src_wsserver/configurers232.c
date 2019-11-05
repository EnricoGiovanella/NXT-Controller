#include <stdio.h>
#include "include/configurers232.h"

struct conf_serialConfData serialConfData;


void setRs232(unsigned char* device, uint32_t baud, uint8_t parity, uint8_t stop, uint8_t data) {
	sprintf(serialConfData.device,"%s",device);

	switch(baud) {
		case 9600:
			serialConfData.baud = B9600;
		break;
		case 19200:
			serialConfData.baud = B19200;
		break;
		case 38400:
			serialConfData.baud = B38400;
		break;
		case 57600:
			serialConfData.baud = B57600;
		break;
		case 115200:
			serialConfData.baud = B115200;
		break;
		default:
			serialConfData.baud = B115200;
		break;
	}

	serialConfData.parity = parity;

	serialConfData.stop = stop;

	switch(data) {
		case 5:
			serialConfData.data = CS5;
		break;
		case 6:
			serialConfData.data = CS6;
		break;
		case 7:
			serialConfData.data = CS7;
		break;
		case 8:
			serialConfData.data = CS8;
		break;
		default:
			serialConfData.data = CS8;
		break;
	}

	fprintf(stderr, "device %s baud %u parity %u stop %u data %u \n",device,baud,parity,stop, data);
}

int setInterfaceAttribs(int fd) {
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if(tcgetattr(fd, &tty) != 0) {
                fprintf(stderr,"error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed(&tty, serialConfData.baud);
        cfsetispeed(&tty, serialConfData.baud);

	
	tty.c_cflag |= (CLOCAL | CREAD);    // ignore modem controls
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= serialConfData.data;         // bit characters
 	//tty.c_cflag &= ~PARENB;     // no parity bit
                        // enable reading
	// No parity
	tty.c_cflag &= ~(PARENB | PARODD);
	// Even parity
	if(serialConfData.parity == 2) tty.c_cflag |= PARENB;
	//Odd parity 
	if(serialConfData.parity == 1) tty.c_cflag |= (PARENB | PARODD);

	//tty.c_cflag &= ~CSTOPB;    // only need 1 stop bit

	// bit stop 2 or 1
	if(serialConfData.stop == 2) tty.c_cflag |= CSTOPB;
        else tty.c_cflag &= ~CSTOPB;

	tty.c_cflag &= ~CRTSCTS;    // no hardware flowcontrol

	// setup for non-canonical mode
	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	tty.c_oflag &= ~OPOST;

	// fetch bytes as they become available
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 5;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		printf("Error from tcsetattr: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}


