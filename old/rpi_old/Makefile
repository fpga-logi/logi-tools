#
#  rpi_loader
#
#  Created by Ting Cao on 3/24/13.
#  Copyright (c) 2013 Ting Cao. All rights reserved.
#
#DEBUG=1
DEBUG=0

ifeq ($(DEBUG),1)
	DEBFLAGS = -ggdb -O0
else
	DEBFLAGS = -O3
endif

CC	= gcc
INCLUDE	= -I/usr/local/include -I./
CFLAGS	= $(DEBFLAGS) -Wall $(INCLUDE) -pipe -fsigned-char

CROSS_COMPILE ?= 
ARCH ?=

CC         = $(CROSS_COMPILE)gcc
CC_COMPILE = $(CC) -c $(OPTFLAGS) $(CFLAGS) $(CC_FLAGS) $(CC_SYSCFLAGS)
CC_LINK    = $(CC) $(LDFLAGS) $(CC_LDFLAGS) $(CC_SYSLDFLAGS)
CC_CFLAGS  =
CC_LDFLAGS =

CC_SYSCFLAGS  =
CC_SYSLDFLAGS =
CC_SYSLIBS    =

CXX            = $(CROSS_COMPILE)g++
CXX_COMPILE    = $(CXX) -c $(OPTFLAGS) $(CFLAGS) $(CXX_FLAGS) $(CXX_SYSCFLAGS)
CXX_LINK       = $(CXX) $(LDFLAGS) $(CXX_LDFLAGS) $(CXX_SYSLDFLAGS)
CXX_CFLAGS     =
CXX_LDFLAGS    =
CXX_FORCE      =
CXX_SYSCFLAGS  =
CXX_SYSLDFLAGS =
CXX_SYSLIBS    =

.cxx.o:	; $(CXX_COMPILE) $<
.cpp.o:	; $(CXX_COMPILE) $<
.cc.o:	; $(CXX_COMPILE) $<

.cxx:
	$(CXX_COMPILE) $<
	$(CXX_LINK) -o $@ $*.o $(LIBRARIES)
.cpp:	
	$(CXX_COMPILE) $<
	$(CXX_LINK) -o $@ $*.o $(LIBRARIES)
.cc:	
	$(CXX_COMPILE) $<
	$(CXX_LINK) -o $@ $*.o $(LIBRARIES)

# Compile plain C code
.c.o:	; $(CC_COMPILE) $<
.c:	
	$(CC_COMPILE) $<
	$(CC_LINK) -o $@ $*.o $(LIBRARIES)

TARGET_RPI_LOADER = logi_loader

TARGET = $(TARGET_RPI_LOADER)

LD = $(CROSS_COMPILE)g++

OBJ_RPI_LOADER = rpi_loader.o smbus.o

all : $(TARGET)

$(TARGET_RPI_LOADER) : $(OBJ_RPI_LOADER)
	$(LD) $(LFLAGS) -o $@ $(OBJ_RPI_LOADER) $(ADDITIONAL_LIBS) 

install: $(TARGET_RPI_LOADER)
	install $(TARGET_RPI_LOADER) /usr/bin

clean :
	rm -f *~ *.o *.a *.so $(OBJ_RPI_LOADER) $(TARGET_RPI_LOADER)

