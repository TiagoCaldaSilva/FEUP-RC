#c compiler
CC := cc

CFLAGS := -Wall -o

BIN := bin/
SRC := src/
INCLUDE := headers/

all: download

download: ${INCLUDE}download.h ${INCLUDE}msgHandler.h ${INCLUDE}utils.h ${SRC}main.c 
		  $(CC) $(CFLAGS) ${BIN}download ${SRC}download.c ${SRC}msgHandler.c ${SRC}utils.c ${SRC}main.c

clean:
	rm -f download