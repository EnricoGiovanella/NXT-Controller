# 'make'        build executable file 'wsserver'
# 'make clean'  removes all .o and executable files

# define the C compiler to use
CC = gcc

# define any libraries to link into executable:
#   if want to link in libraries (libx.so or libx.a) use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS = -lwebsock -lpthread

# define any directories containing header files other than /usr/include
#
IDIR = include

# defines dependencies from .h files
#
_DEPS_WSSERVER = wsserver.h tools.h receivewsmsg.h usbrs232.h variables.h
DEP_WSSERVER = $(patsubst %,$(IDIR)/%,$(_DEPS_WSSERVER))

_DEPS_USBR232 = usbrs232.h wsserver.h variables.h
DEP_USBR232 = $(patsubst %,$(IDIR)/%,$(_DEPS_USBR232))

_DEPS_TOOLS = tools.h variables.h
DEP_TOOLS = $(patsubst %,$(IDIR)/%,$(_DEPS_TOOLS))

_DEPS_MESSAGES = messagews.h wsserver.h tools.h receivewsmsg.h trasmitwsmsg.h actionwsmsg.h variables.h
DEP_MESSAGES = $(patsubst %,$(IDIR)/%,$(_DEPS_MESSAGES))

_DEPS_ACTIONWSMSG = actionwsmsg.h messagews.h trasmitwsmsg.h variables.h
DEP_ACTIONWSMSG = $(patsubst %,$(IDIR)/%,$(_DEPS_ACTIONWSMSG))



all: wsserver

wsserver: wsserver.o usbrs232.o tools.o messagews.o actionwsmsg.o
	 $(CC) -o wsserver wsserver.o usbrs232.o tools.o messagews.o actionwsmsg.o $(LIBS)

# define the C object files 
wsserver.o: wsserver.c $(DEP_WSSERVER)
	 $(CC) -c wsserver.c

usbrs232.o: usbrs232.c $(DEP_USBR232)
	 $(CC) -c usbrs232.c 

tools.o: tools.c $(DEP_TOOLS)
	 $(CC) -c tools.c

messagews.o: messagews.c $(DEP_MESSAGES)
	 $(CC) -c messagews.c

actionwsmsg.o: actionwsmsg.c $(DEP_ACTIONWSMSG)
	 $(CC) -c actionwsmsg.c

# 'make clean'  removes all .o and executable files
clean:
	 rm wsserver *.o 
