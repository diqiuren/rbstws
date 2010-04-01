# Make file
#
# $Id: Makefile 4 2010-01-10 22:10:13Z adias $

#CC := avr32-linux-gcc
CFLAGS := -pipe -O2 -g -Wall -pthread

DEPS = connection.h
OBJ = server.o connection.o parser.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	
.PHONY: clean
clean:
	$(RM) *.o *~ server
