CC=gcc
CFLAGS=-D_GNU_SOURCE -g
all: showptr.c 
	${CC} ${CFLAGS} -o showptr showptr.c
