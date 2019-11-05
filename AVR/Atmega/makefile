# 'make'        build executable file 'wsserver'
# 'make clean'  removes all .o and executable files

# define the C compiler to use
CC = avr-gcc
CCOBJCOPY = avr-objcopy

CFLAGS = -O1 -std=gnu11 -w #-Wall
CFLAGSOBJ = -O ihex -j .text -j .data

MMCU = -mmcu=atmega2560

CLOCK = -DF_CPU=16000000

# define any libraries to link into executable:
#   if want to link in libraries (libx.so or libx.a) use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
#LIBS = -lwebsock -lpthread


# defines dependencies from .h files
#
DEPS_NXTMOTOR = atmega2560.h atmega2560data.h message.h message_inline.h feedback.h feedback_inline.h

DEPS_ATMEGA2560 = atmega2560.h atmega2560data.h

DEPS_ATMEGA2560DATA = atmega2560data.h

DEPS_FEEDBACK = feedback.h

DEPS_MESSAGE = message.h atmega2560data.h


all: nxtmotor

nxtmotor: nxtmotor.elf
	$(CCOBJCOPY) $(CFLAGSOBJ) nxtmotor.elf nxtmotor.hex

nxtmotor.elf: nxtmotor.o atmega2560.o atmega2560data.o feedback.o message.o
	 $(CC) -o nxtmotor.elf nxtmotor.o atmega2560.o atmega2560data.o feedback.o message.o $(MMCU) $(CLOCK) $(CFLAGS)

# define the C object files 
nxtmotor.o: nxtmotor.c $(DEPS_NXTMOTOR)
	 $(CC) -c nxtmotor.c $(MMCU) $(CLOCK) $(CFLAGS)

atmega2560.o: atmega2560.c $(DEPS_ATMEGA2560)
	 $(CC) -c atmega2560.c $(MMCU) $(CLOCK) $(CFLAGS)

atmega2560data.o: atmega2560data.c $(DEPS_ATMEGA2560DATA)
	 $(CC) -c atmega2560data.c $(MMCU) $(CLOCK) $(CFLAGS)

feedback.o: feedback.c $(DEPS_FEEDBACK)
	 $(CC) -c feedback.c $(MMCU) $(CLOCK) $(CFLAGS)

message.o: message.c $(DEPS_MESSAGE)
	 $(CC) -c message.c $(MMCU) $(CLOCK) $(CFLAGS)

# 'make clean'  removes all .o and executable files
clean:
	 rm nxtmotor.elf nxmotor.hex *.o

