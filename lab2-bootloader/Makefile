CC = gcc
CFLAGS = -Wall -g 

SRC = my-teensy-install.c ../../lib_teensy/cs140e-teensy-tty.c
OBJS = $(SRC:.c=.o) 

all: my-teensy-install

my-teensy-install: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o my-teensy-install -lm

clean :
	rm -f *.o *~ Makefile.bak my-teensy-install