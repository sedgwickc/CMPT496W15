# Charles Sedgwick
# CMPT 496 W15
# Makfile to build Restarter

CC=gcc
CFLAGS=-Wall -std=c99 
DEBUG=-g -DDEBUG
LINK=
LIBNL=$(shell pkg-config --libs libnl-3.0 )
#CFLAGS+=$(shell pkg-config --cflags libnl-3.0 )

all: restarter

debug: restarter_debug

restarter: restarter.c
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(LIBNL) $^ -o $@ 

%_db.o: %.c
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $^

clean:
	rm *.o restarter restarter_debug
