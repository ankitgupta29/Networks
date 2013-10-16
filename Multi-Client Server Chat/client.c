#if 0
Authors:
Ankit Gupta MSCS Fall 2012 UIN:621009649
Osama Arshad UIN:315004499
Date:18 Sept 2012
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SBCPHeader.h"
//#include "client_info.c"
#define VRSN 3
#define STDIN 0
#define STDOUT 1
bool idlesend=false;

int MakePacket(short int mtype,short int attype,char **buf2,char* extrainfo)
{       
/*
SBCPMsg* MsgPtr =  buf1;
	 SBCPAttr* attr =  (buf1+4);
	 short int x=MsgPtr->vrsntype;
	 short int mtype=127 & x;
	 short int version= (x>>7)&1023;
          printf("mtype is %d\n",mtype);

*/
 #if 0
        short int vertype=  VRSN<<7 | mtype;
	short int msglen = 4+4+strlen(extrainfo);
        printf("lenegth of extrainfo is %d, msl is %d",strlen(extrainfo),msgl);
	short int attl = 4+strlen(extrainfo);
	
	*buf2 =(char*)malloc(msgl);
	memset(*buf2,0,msgl);
	short int *x= &temp;
	memcpy((*buf2),x,2); //vrsntype
	x = &msgl;
	memcpy((*buf2)+2,x,2);
	x = &attype;		
	memcpy((*buf2)+4,x,2);
	x = &attl;
	memcpy((*buf2)+6,x,2);
	memcpy((*buf2)+8,extrainfo,strlen(extrainfo));
#endif
	SBCPMsg msgh;
	SBCPAttr atrh;
	msgh.vrsntype =  (VRSN<<7) | mtype;
	msgh.mlength  =  4+4+strlen(extrainfo)+1;
	atrh.type     =  attype;
	atrh.alength  =  4+strlen(extrainfo)+1;
	
	short int msglen = msgh.mlength;
	//data copy start
	*buf2 =(char*)malloc(msglen);
	memset(*buf2,0,msglen);
	//printf("done allocation\n");
	memcpy(*buf2,&msgh,4);
	memcpy(*buf2+4,&atrh,4);
	memcpy(*buf2+8,extrainfo,strlen(extrainfo)+1);
	//int i;
        //printf("byte[%d] %x \n",i,(*buf2)[i]);
        //printf("extra info name is  %s\n",extrainfo);
	return msglen;
}	

void ExtractPacket(char *buf1)
{
/*check for att type,
1. if mtype = ack 7,then attype=message and client count/Username and should display the payload to stdoutput
2  if mtype = nak 5 , then client display the reason in payload for exiting client.
3  if mtype = online 8, then display username in payload which is online now.
4  if mtype = offline 6, then display username in payload which is offline now.
5  if mtype = idle 9, then display username in payload which is idle now.
6  if mtype = fwd 3, client display username:chat on stdoutput
*/
 SBCPMsg* MsgPtr =  buf1;
 //SBCPAttr* attr =  (buf1+4);
 short int mtype=127 & (MsgPtr->vrsntype);
 //short int vrsn= (x>>7)&1023;
 //short int msglen =MsgPtr->mlength;
 //short int attlen =attr->alength;

 //char *reason=NULL;
 //int i,plen;
 //int attlen2 = attr2; 
 //printf("Inside Extract packet Client\n");
 //for(i=0;i<msglen;i++)	
 //printf("byte[%d]: %x \n",i,(buf1)[i]);
 //printf(" mslen is %d\n",msglen);
 //printf("Selecting Switch type\n");

 switch(mtype)
 {
  case 3:
	 //printf("PRINTING CHAT MSG\n"); 
         printf("%s",(buf1+8)); 
	 break;

  case 5://NAK MSG
	 //printf("PRINTING NAK MSG\n");
         printf("[%s]:",(buf1+8));
         printf("MSG-NAK:Closing Connection\n");
    	 break;

  case 6://OFFLINE MSG
         //printf("PRINTING OFFLINE MSG\n"); 
   	 printf("[%s]:",(buf1+8));
         printf(" This User is offline now !!\n");
	 break;

  case 7://ACK MSG
	 printf("ACK Received\n");
	 printf("ClientCount : Users\n"); 
	 printf("%s",(buf1+8));
         printf("\n");
	 break;

  case 8://ONLINE MSG
         //printf("PRINTING ONLINE MSG\n"); 
   	 printf("[%s]:",(buf1+8));
         printf(" This User is Online now !!\n");
	 break;

  case 9://IDLE MSG
   	 //printf("PRINTING IDLE MSG\n"); 
	 printf("[%s]:",(buf1+8));
         printf(" This User is IDLE now !!\n");
	break;
  default:
        printf("Unrecognized Packet type\n"); 
        break;

 }//end of switch

}//end of ExtractPacket


void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buf[256];
    if (argc < 4) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[3]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    char *name=NULL;
    int namelen =  strlen(argv[1]); 
    name=(char*)malloc(namelen+1);
    memset(name,0,namelen+1);
    memcpy(name,argv[1],namelen);
    name[namelen] = '\0';
    //printf("name here is %s\n",name);
    if (sockfd < 0) 
     {
	error("ERROR opening socket\n");
        exit(0);
     }
    server = gethostbyname(argv[2]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
     {
        error("ERROR connecting\n");
	exit(0);
     }
     //int i,nbytes,j;
     printf("\n********* Connected to the Server *********** \n"); 
     //if connect successful send JOIN msg to server
     short int typejoin =2; //set msg type as JOIN
     char *buffer=NULL;
     short int attype = 2; //attribute type=2 for username
     n = MakePacket(typejoin,attype,&buffer,name);
     //change the length of the data to be sent  
     //printf("Packet Done \n");
     if (send(sockfd, buffer,n, 0) == -1) perror("error in sending JOIN MSG\n");
     
     fd_set readSet,master_list_read;
     FD_ZERO(&master_list_read);
     FD_SET(STDIN, &master_list_read);//stdin manually trigger reading
     FD_SET(sockfd, &master_list_read);//tcp socket

     struct timeval tv;
     int retval;
     tv.tv_sec = 10;
     tv.tv_usec = 0;
     while(1)
     { 
	   char *buffer=NULL;
	   int n;

	   tv.tv_sec = 10;
	   readSet=master_list_read;
	   retval = select(sockfd+1, &readSet, NULL, NULL,&tv);
	   if (retval == -1) 
	   {
		perror("Error in select\n");
		exit(4);
	   }
	   else if(retval==0)
		{ 
		  if(!idlesend)
		  {
		   buffer=NULL;
		   short int typeidle =9; //set msg type as SEND
		   short int attuname = 2; //attribute type=2 for username
		   sprintf(buf,"%s",name);
		   n =MakePacket(typeidle,attuname,&buffer,name);
		   printf("!! Going Idle !!\n");
		   idlesend=true;
                   if (send(sockfd, buffer,n, 0) == -1) 
			perror("error in sending MSG to server\n");
		       //printf("*********SEND SUCCESSFUL******\n");
		    //free(buffer);
                  }
		}		
		else 
	       	 {	
		    if(FD_ISSET(0, &readSet))
		   {
		    idlesend=false;
		    bzero(buf,256);
		    fgets(buf,256,stdin); 
		    //buf[512]='\0';
		    /*for(i=0;i<5;i++)	
		    printf("byte[%d]: %c \n",i,buf[i]);*/
		    short int typesend =4; //set msg type as SEND
		    short int attmsg = 4; //attribute type=4 for message
		    int n =MakePacket(typesend,attmsg,&buffer,buf);
		    if (send(sockfd, buffer,n, 0) == -1) perror("error in sending MSG to server\n");
		       //printf("*********SEND SUCCESSFUL******\n");
		    //free(buffer);
		  }

		  else  if (FD_ISSET(sockfd, &readSet))  // receives data from server,print it on StdOutput
	 	  {  
		    bzero(buf,256);
		    int nbytes = recv(sockfd, buf, 256,0);
		    if(nbytes<=0)
		     {
		       perror("recv error from server \n");
		       exit(0);
		     }
		    ExtractPacket(buf);
		  }
	
	      } //END OF ELSE 
	    } //end of while
    close(sockfd);          
    return 0;
}
