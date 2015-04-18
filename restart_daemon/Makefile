# Charles Sedgwick
# CMPT 496 W15
# Makfile to build Restarter

CC=gcc
CFLAGS=-Wall -std=c99 -DMEMWATCH -DMW_STDIO -c
PTHREAD=-DMW_PTHREADS
DEBUG=-g -DDEBUG
LINK_PTHREAD=-lpthread

all: restarter

debug: restarter_debug

restarter: restarter.o logging.o memwatch.o
	$(CC) $^ -o $@

restarter_db: restarter_db.o logging_db.o memwatch.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $^

%_db.o: %.c
	$(CC) $(CFLAGS) $(DEBUG) $^

clean:
	rm *.o restarter restarter_debug
