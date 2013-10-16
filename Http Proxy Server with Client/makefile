# CC Compiler to use is GCc
CC=gcc

# CFLAGS are the flags which are passed to compiler
CFLAGS=-Wall -g 

all:  server client
 server: server.c cache.c
	$(CC) $(CFLAGS) -o server server.c 
 client: client.c 
	$(CC) $(CFLAGS) -o client client.c
clean:
	rm -rf *o server client


