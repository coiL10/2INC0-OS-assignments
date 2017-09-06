#
#	Makefile for 2INC0 Interprocess Communication
#
#	(c) TUe 2010-2017, Joris Geurts
#

BINARIES = worker farmer md5s

CC = gcc
CFLAGS = -Wall -g -c
LDLIBS = -lrt -lm

all:	$(BINARIES)

clean:
	rm -f *.o $(BINARIES)

worker: worker.o md5s.o

farmer: farmer.o

md5s: md5s.o md5s_main.o

worker.o: worker.c common.h

farmer.o: farmer.c settings.h common.h

