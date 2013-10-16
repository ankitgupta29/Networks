#if 0
Authors:
Ankit Gupta MSCS Fall 2012 UIN:621009649
Osama Arshad UIN:315004499
ECEN 602 HW2 TFTP
Date:25 Sept 2012
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

typedef struct 
{
	short int opcode;
	char *filename;
	char eos;
	char *mode;
}READReq;

typedef struct
{
	short int opcode;
	short int blockno;
}DATAReq;

typedef struct
{
	short int opcode;
	short int lastackblock;
}ACKReceived;

typedef struct 
{
	short int opcode;
	short int errorcode;
}ERRORSend;


typedef struct
{
	FILE *fp;
 	int blockno;
 	unsigned long filelength;
        int time;
        struct sockaddr_in clistr;
        int newsocket;
	struct Clients * next;
	
}Clients;
