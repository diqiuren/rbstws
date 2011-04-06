# Make file for Apollo example
CROSS_COMPILE := avr32-linux-
CC := $(CROSS_COMPILE)gcc
CFLAGS := -pipe -O2 -g -Wall

DEPS = jtag.h
OBJ = jtag.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

jtag: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	$(RM) *.o *~ jtag
