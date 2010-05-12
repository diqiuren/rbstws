# Makefile
#
# $Id: Makefile 11 2010-04-24 17:16:16Z americo.dias $

REV=$(shell svn info --revision HEAD | grep 'Revision: ' | sed 's/[^0-9]*\([0-9]\+\).*/\1/')
#REV=0
CC := avr32-linux-gcc
CFLAGS := -pipe -O2 -g -Wall -pthread -fno-strict-aliasing -D_REENTRANT -D_REV=$(REV)

DEPS = parser.h timestamp.h log.h data_conn.h command_conn.h defs.h jtag.h interpreter.h
OBJ = main.o parser.o timestamp.o log.o data_conn.o command_conn.o jtag.o interpreter.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

rbstws: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	
.PHONY: clean
clean:
	$(RM) *.o *~ rbstws
