#c compiler
CC := cc

CFLAGS := -Wall -o

BIN := bin/
SRC := src/
INCLUDE := headers/

all: app

app: ${INCLUDE}application_layer.h ${INCLUDE}link_layer.h ${INCLUDE}receiver.h ${INCLUDE}sender.h ${INCLUDE}utils.h ${SRC}application_layer.c 
	$(CC) $(CFLAGS) ${BIN}app ${SRC}link_layer.c ${SRC}utils.c ${SRC}sender.c ${SRC}receiver.c ${SRC}application_layer.c -lm

clean:
	rm -f app