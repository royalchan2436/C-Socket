CC = gcc
PORT = 55651
CFLAGS = -DPORT=$(PORT) -Wall -g

all: poll_server 

poll_server: poll_server.o lists.o
	${CC} ${CFLAGS} -o $@ poll_server.o lists.o

poll_server.o: poll_server.c lists.h
	${CC} ${CFLAGS} -c $<

%.o: %.c
	${CC} ${CFLAGS} -c $<

clean: 
	rm polls *.o
