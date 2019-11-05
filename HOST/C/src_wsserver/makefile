# 'make'        build executable file 'wsserver'
# 'make clean'  removes all .o and executable files

# define the C compiler to use
CC = gcc

CFLAGS = -O1

# define any libraries to link into executable:
#   if want to link in libraries (libx.so or libx.a) use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS = -lwebsock -lpthread

# define any directories containing header files other than /usr/include
#
IDIR = include

# defines dependencies from .h files
#
_DEPS_WSSERVER = wsserver.h usbrs232.h configurers232.h
DEP_WSSERVER = $(patsubst %,$(IDIR)/%,$(_DEPS_WSSERVER))

_DEPS_MESSAGE = message.h actionwsmsg.h wsserver.h
DEP_MESSAGE = $(patsubst %,$(IDIR)/%,$(_DEPS_MESSAGE))

_DEPS_USBR232 = usbrs232.h message.h dllist.h timertx.h configurers232.h actionmsgavr.h
DEP_USBR232 = $(patsubst %,$(IDIR)/%,$(_DEPS_USBR232))

_DEPS_ACTIONMSGAVR = actionmsgavr.h message.h usbrs232.h configurers232.h
DEP_ACTIONMSGAVR = $(patsubst %,$(IDIR)/%,$(_DEPS_ACTIONMSGAVR))

_DEPS_CONFIGURERS232 = configurers232.h 
DEP_CONFIGURERS232 = $(patsubst %,$(IDIR)/%,$(_DEPS_CONFIGURERS232))

_DEPS_DLLIST = dllist.h 
DEP_DLLIST = $(patsubst %,$(IDIR)/%,$(_DEPS_DLLIST))

_DEPS_TIMERTX = timertx.h actionmsgavr.h
DEP_TIMERTX = $(patsubst %,$(IDIR)/%,$(_DEPS_TIMERTX))

_DEPS_ACTIONWSMSG = actionwsmsg.h message.h usbrs232.h
DEP_ACTIONWSMSG = $(patsubst %,$(IDIR)/%,$(_DEPS_ACTIONWSMSG))

#_DEPS_AVRMESSAGE = avrmessage.h actionmsgavr.h
#DEP_AVRMESSAGE = $(patsubst %,$(IDIR)/%,$(_DEPS_AVRMESSAGE))

#_DEPS_TOOLS = tools.h variables.h
#DEP_TOOLS = $(patsubst %,$(IDIR)/%,$(_DEPS_TOOLS))

#_DEPS_MESSAGES = messagews.h wsserver.h tools.h receivewsmsg.h trasmitwsmsg.h actionwsmsg.h variables.h
#DEP_MESSAGES = $(patsubst %,$(IDIR)/%,$(_DEPS_MESSAGES))




all: wsserver

wsserver: wsserver.o message.o usbrs232.o actionmsgavr.o actionwsmsg.o configurers232.o dllist.o timertx.o
	 $(CC) -o wsserver wsserver.o message.o usbrs232.o actionmsgavr.o actionwsmsg.o configurers232.o dllist.o timertx.o $(LIBS) $(CFLAGS)

# define the C object files 
wsserver.o: wsserver.c $(DEP_WSSERVER)
	 $(CC) -c wsserver.c

message.o: message.c $(DEP_MESSAGE)
	 $(CC) -c message.c

usbrs232.o: usbrs232.c $(DEP_USBR232)
	 $(CC) -c usbrs232.c

actionmsgavr.o: actionmsgavr.c $(DEP_ACTIONMSGAVR)
	 $(CC) -c actionmsgavr.c

actionwsmsg.o: actionwsmsg.c $(DEP_ACTIONWSMSG)
	 $(CC) -c actionwsmsg.c

configurers232.o: configurers232.c $(DEP_CONFIGURERS232)
	 $(CC) -c configurers232.c

dllist.o: dllist.c $(DEP_DLLIST)
	 $(CC) -c dllist.c

timertx.o: timertx.c $(DEP_TIMERTX)
	 $(CC) -c timertx.c

#avrmessage.o: avrmessage.c $(DEP_AVRMESSAGE)
#	 $(CC) -c avrmessage.c

#messagews.o: messagews.c $(DEP_MESSAGES)
#	 $(CC) -c messagews.c

# 'make clean'  removes all .o and executable files
clean:
	 rm wsserver *.o 
#include/*.gch

