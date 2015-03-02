CC=gcc
CFLAGS=-Wall -std=c99
DEBUG=-g -DDEBUG
EXEC= alloc_mem

all: $(EXEC)

alloc_mem: alloc_mem.o oom_exp.o
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $^

%.o: %.c 
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm *.o $(EXEC)
