# CC Compiler to use is GCC
CC=gcc
# CFLAGS are the flags which are passed to compiler
CFLAGS=-Wall -g

 server: server.c SBCPHeader.h
	$(CC) $(CFLAGS) -o server server.c 

 client: client.c SBCPHeader.h
	$(CC) $(CFLAGS)  client.c -o client 


clean:
	rm -rf *o server client


    
    
    
