# Charles Sedgwick
# CMPT 496 W15
# Makfile to build Restarter

CC=gcc
CFLAGS=-Wall -std=c99 -c 
DEBUG=-g -DDEBUG
LINK=
LIBNL=$(shell pkg-config --libs libnl-3.0 )
CFLAGS+=$(shell pkg-config --cflags libnl-3.0 )

all: restarter

debug: restarter_debug

restarter: restarter.o
	$(CC) $(LINK) $^ -o $@

restarter_libnl: restarter_libnl.o
	$(CC) $(LIBNL) $(LINK) $^ -o $@

restarter_debug: restarter_db.o
	$(CC) $(LINK) $^ -o $@

restarter_2:
	gcc -Wall -std=c99 restarter_2.c -o restarter

%.o: %.c
	$(CC) $(CFLAGS) $(LIBNL) $^ -o $@ 

%_db.o: %.c
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $^

clean:
	rm *.o restarter restarter_debug
