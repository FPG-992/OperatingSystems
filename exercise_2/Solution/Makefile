CC=gcc
CFLAGS=-Wall

all: gates child

gates: parent.c
	$(CC) $(CFLAGS) $^ -o $@

child: child.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f gates child
