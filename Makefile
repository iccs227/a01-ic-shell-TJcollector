CC=gcc
CFLAGS=-Wall -g 
BINARY=icsh

all: icsh

icsh: icsh.c game.c
	$(CC) -o $(BINARY) $(CFLAGS) icsh.c game.c

.PHONY: clean

clean:
	rm -f $(BINARY)
