#if 0
Authors:
Ankit Gupta MSCS Fall 2012 UIN:621009649
Osama Arshad UIN:315004499
ECEN 602 HW2 TFTP
Date:25 Sept 2012
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "TFTPHeader.h"
#include <string.h>
#include <stdbool.h>
#define MAXPORT 30000

/*
* This function reports the error and
* exits back to the shell:
*/

int inc=0;
static void bail(const char *on_what) {
fputs(strerror(errno),stderr);
fputs(": ",stderr);
fputs(on_what,stderr);
fputc('\n',stderr);
exit(1);
}

Clients *head=NULL;

/*
*returns NULL if not found else ptr to clientlist for found
*/
FILE* search_client(int sockfd)
{
 if(head==NULL) {return 0;} 
 Clients * temp=head;
 while(temp!=NULL)
 { 
  if(temp->newsocket == sockfd)
  {  
    // printf("SAme client matches returning \n");
     return temp->fp;
  }
  else
   temp=(Clients*)temp->next;
 }
 return NULL;
}

/*
* Add client information.Deails
*/
int add_Client(FILE * fileptr,int block,unsigned long filelen,struct sockaddr_in adr_clnt,int newsock,int time,Clients* next)
{
  //printf("Inside Add_client\n");
  Clients * temp=head;
  Clients *newclient = (Clients*)malloc(sizeof(Clients));
  if(newclient==NULL)
  {
   printf("Error in Allocating Memory to new client\n");
   return -1;
  }
  newclient->fp=fileptr;
  newclient->blockno=block;
  newclient->filelength=filelen;
  newclient->next=NULL;
  newclient->clistr=adr_clnt;
  newclient->newsocket=newsock;
  newclient->time = time;
  if(temp==NULL) //first element in a list to be added
  { 
   head=newclient;
  }
  else //put element at last
  {
   while(temp->next!=NULL)
	temp=(Clients*)temp->next;
   temp->next=newclient;
  }
//printf("returning from add_client\n");
return 0;
}

/*
* set blockno against sockfd
*/
void setblockno(int sockfd,short int block)
{
 Clients *temp=head;
 while(temp!=NULL)
 {
  if(temp->newsocket==sockfd)
    {
      temp->blockno = block;      
      return;
    }
  else
   temp=(Clients*)temp->next;
 }
return;
}

/*
* get last block no send to client
*/
int getlastblockno(int sockfd)
{
 Clients *temp=head;
 while(temp!=NULL)
 {
  if(temp->newsocket==sockfd)
    {
      printf("Value of temp->block in getlastblock is %d\n",temp->blockno);
      return temp->blockno;
    }
  else
   temp=(Clients*)temp->next;
 }
 return 0;
}

/*
* return structure against sockfd
*/
struct sockaddr_in getstructure(int sockfd)
{
 Clients *temp=head;
 while(temp!=NULL)
 {
  if(temp->newsocket==sockfd)
    {
      printf("Value of temp->newsocket matches and return sockaddr");
      return temp->clistr;
    }
  else
   temp=(Clients*)temp->next;
 }
}

/*
* Update time field in Client structure to make zero if mkzero is true else ++1
*/
int updatetimetozero(int sockfd,bool mkzero)
{
 Clients *temp=head;
 while(temp!=NULL)
 {
  if(temp->newsocket==sockfd)
    { 
      printf("Value of sockfd is %d\n",temp->newsocket);
      printf("Value of temp->time is %d\n",temp->time);
      if(mkzero)
       {   
    	temp->time = 0;
        return temp->time; 
       }
      else
       {
       (temp->time)++;
       return temp->time;
       }
    }
  else
   temp=(Clients*)temp->next;
 }
 return 0;
}

/*
* Read from file with block number request.
*/
int readfile(FILE *file,int block,char **buffer)
{
        printf("Inside Readfile with block %d\n",block);
	unsigned long fileLen;
	int nbytes=0;
	//Get file length
	fseek(file, 0, SEEK_END);
	fileLen=ftell(file);
	fseek(file, 0, SEEK_SET);
        printf("filelen is %ld\n",fileLen);
	if(fileLen > (block)*512)
         { 
	   nbytes = 512;
    	   *buffer=(char *)malloc(512);
	 }
        else 
	 {
           *buffer=(char*)malloc(fileLen-(block-1)*512);
	    bzero(*buffer,fileLen-(block-1)*512);
	    nbytes = fileLen-(block-1)*512;
	 }
	if (!*buffer)
	{
		fprintf(stderr, "Memory error!");
                fclose(file);
		return;
	}
	//Read file contents into buffer
	fseek(file,(block-1)*512,SEEK_SET);
	fread (*buffer,1,nbytes,file);
	printf("Read file and buffer inside readfile is nbyte = %d \n",nbytes);
	//int i;
        //for(i=0;i<nbytes;i++)
        //printf("byte[%d] %x \n",i,(*buffer)[i]);
        return nbytes;
}

/*
* Make Error Packet
*/
int MakeErrorPacket(short int opcode,short int errorcode,char *errstring,char **buf)
 { 
  printf("Inside send error and errstring in MakeErrorPacket is %s\n",errstring);
  ERRORSend setError;
  setError.opcode    = htons(opcode);//opcode=5 for error
  setError.errorcode = htons(errorcode);//errorcode=0 not defined
  int msglen = 5+strlen(errstring);
  *buf =(char*)malloc(msglen);
  bzero(*buf,msglen);
  memcpy(*buf,&setError,4);
  memcpy(*buf+4,errstring,strlen(errstring));
  printf("returning MakeErrorPacket with msglen is %d \n",msglen);
  //int i;
  // for(i=0;i<msglen;i++) 
  //printf("byte[%d] %x \n",i,(*buf)[i]);
  return msglen;
}

/*
* Make data Packet
*/
int MakeDataPacket(short int opcode,int block,char *sendbuf,char **buf,int nbytes)
 { 
  printf("entering makpacket from MakeDataPacket");
  printf("Inside MakeDataPacket and opcode is %d,block is %d \n",opcode,block);
  DATAReq datasend;
  datasend.opcode    = htons(opcode);//opcode=5 for error
  datasend.blockno   = htons(block); 
  //datasend.data=(char*)malloc(nbytes);
  int msglen = 4+nbytes;
  //printf("Msglen is %d\n",msglen);
  *buf =(char*)malloc(msglen);
  bzero(*buf,msglen);
  memcpy(*buf,&datasend,4);
  memcpy(*buf+4,sendbuf,nbytes);
  printf("returning from MakeDataPacket and msglen is %d and buf is \n",msglen);
	 //int i;
	 //for(i=0;i<msglen;i++)
	 //printf("byte[%d] %x \n",i,(*buf)[i]);
  return msglen;
}

/*
* Takes the Buf from receiver and check for opcode first
* if it is readreq, then send filename in extrainfo and return opcode
* if it is ack, then send block number to be read in extrainfo and return opcode
* if error, then set extrainfo as errstring and return errorcode =5
*/
int ExtractPacket(char *buf,char** extrainfo)
 {
  printf("inside Extract packet \n"); 
  READReq *ptr = (READReq*)buf;
  short int opcode;
  opcode       = ntohs(ptr->opcode);
  printf("Opcode here is %d\n",opcode);
  if(opcode==1) //read request
  { 
    char *fname =  (char*)malloc(40);
    bzero(fname,40);
    strcpy(fname,buf+2);
    printf("ReadReq from client for filename %s\n",fname);
    int filelength = strlen(fname);
    char *modetype =  (char*)malloc(40);
    bzero(modetype,40); 
    strcpy(modetype,buf+2+filelength+1);
    printf("ReadReq from client for modetype is %s\n",modetype); 
    if(strcmp(modetype,"octet"))
    {
	 printf("Mode not supported\n");//send errorpacket stating invalid mode.
         char *errstring ="Mode not supported";
	 short int errorcode =5;
	 *extrainfo=(char*)malloc(strlen(errstring));
	 bzero(*extrainfo,strlen(errstring)); 
	 strcpy(*extrainfo,errstring);
          
	 return errorcode;
    }
    
    *extrainfo=(char*)malloc(strlen(fname));
    bzero(*extrainfo,strlen(fname));   
    //memcpy(*extrainfo,fname,strlen(fname));
    strcpy(*extrainfo,fname);
    return opcode;
  }//end of if opcode==2

  else if(opcode == 4) //ack received
  {
    //   opcode|block no 
    //      2  |  2
    ACKReceived *ack = (ACKReceived*)buf; 
    short int blockread = ntohs(ack->lastackblock);
    printf("Block read is here in extract packet is %d",blockread);
    *extrainfo =(char*)malloc(2);
    bzero(*extrainfo,2);
    sprintf(*extrainfo,"%d",blockread);
    return opcode;
    
  }
  else //if opcode doesnt match to 2 and 4
  {
    printf("Invalid Opcode Request.Opcode is %d\n",opcode);
    char *errstring= "Invalid Opcode Request";
    short int errorcode =5;
    *extrainfo =(char*)malloc(strlen(errstring)); 
    bzero(*extrainfo,strlen(errstring)); 
    memcpy(*extrainfo,errstring,strlen(errstring)); 
    return errorcode;
  }
  return opcode;
} //end of extract Packet


int main(int argc,char **argv) 
{
int z;
char *srvr_addr = NULL;
struct sockaddr_in adr_inet;/* AF_INET */
struct sockaddr_in adr_clnt;/* AF_INET */
int len_inet;/* length */
int s,ss;/* Socket */
char dgram[516];/* Recv buffer */
char *buffer=NULL;
time_t td;/* Current Time and Date */
struct tm tm;/* Date time values */

/*
* Use a server address from the command
* line, if one has been provided.
* Otherwise, this program will default
* to using the arbitrary address
* 127.0.0.23:
*/
if ( argc < 3 ) {/* Addr on cmdline: */
bail("Please enter all the fucking arguments.");
} 
srvr_addr = argv[1];
/*
* Create a UDP socket to use:
*/
s = socket(AF_INET,SOCK_DGRAM,0);
if ( s == -1 )
bail("socket()");
/*
* Create a socket address, for use
* with bind(2):
*/
memset(&adr_inet,0,sizeof adr_inet);
adr_inet.sin_family = AF_INET;
adr_inet.sin_port = htons(atoi(argv[2]));
adr_inet.sin_addr.s_addr =
inet_addr(srvr_addr);
if ( adr_inet.sin_addr.s_addr == INADDR_NONE )
	bail("bad address.");
len_inet = sizeof adr_inet;
/*
* Bind a address to our socket, so that
* client programs can contact this
* server:
*/
z = bind(s,(struct sockaddr *)&adr_inet,len_inet);
if ( z == -1 )
	bail("bind()");
printf("Bind sucess to local port %s\n ",argv[2]);
/*
* Now wait for requests:
*/
 
 fd_set read_fds,master_list;
 FD_ZERO(&master_list);
 FD_ZERO(&read_fds);
 FD_SET(s,&master_list);
 int retval,i,timeout=0,lastsend,n,nbytes,fdmax = s; // so far, it's this one
 char *sendbuf=NULL;
 FILE *file;
for (;;) 
{
 /*
 * Block until the program receives a
 * datagram at our address and port:
 */
  struct timeval tv;
  int retval;
  tv.tv_sec = 5;
  tv.tv_usec = 0;
  read_fds = master_list; // copy it
  retval = select(fdmax+1, &read_fds, NULL, NULL, &tv);
  if(retval == -1)
   {
     perror("select");
     exit(4);
   }
  else if(retval == 0)
   {
   for(i=0;i<=fdmax;i++)
     { 
      if(FD_ISSET(i, &read_fds)) 
      {  
       if(i!=s)
        {
         timeout=updatetimetozero(i,false);
         if(timeout >= 6) //have to resend the lastsendpacket //that makes total timeout 5*6=30 sec
          {
             lastsend =getlastblockno(i);
	     file = search_client(i);
             struct sockaddr_in clnt_adr = getstructure(i);
             len_inet = sizeof clnt_adr;
	     nbytes = readfile(file,lastsend,&sendbuf);
             n = MakeDataPacket(3,lastsend,sendbuf,&buffer,nbytes);
             z = sendto(i,/* Socket to send result */buffer, n, /* The datagram lngth */
		 0,/* Flags: no options */(struct sockaddr *)&clnt_adr,/* addr */len_inet); /* Client address length */
	     if ( z < 0 )
	        bail("sendto(2)");
	     updatetimetozero(i,true);
             if(z<512)
	     {
	      printf("File Copied Successfully to Socket :%d\n",i);
	      close(i);
              FD_CLR(i,&master_list);
	     }  
          }//end of if timeout >=6      
        }//end of i!=s
      }//end of FD_ISSET
     }//end of looping on fds
    }//end of else retval =0
  else
   {
     for(i=0;i<=fdmax;i++)
     {
       if (FD_ISSET(i, &read_fds))
        {
	    printf("if FD isset for port %d \n",i);
            len_inet = sizeof adr_clnt;
	    z = recvfrom(i,/* Socket */dgram,/* Receiving buffer */sizeof dgram,/* Max recv buf size */
	        	0,/* Flags: no options */(struct sockaddr *)&adr_clnt,/* Addr */&len_inet);/* Addr len, in & out */
            printf("Value of z is %d\n",z);
	    if ( z < 0 )
		bail("recvfrom(2)");
	    /*
	    extract the packet here
	    */
	    char  *extrainfo =NULL;
            short int opcode = ExtractPacket(dgram,&extrainfo);
	    //printf("after extractpacket : opcode is %d and extrainfo is %s\n",opcode,extrainfo);
	    int z,blockno,buflen,block = 1,lastblockread=1;
            unsigned short newfd;
	    char  *errstring=NULL;
            unsigned long filelen;
            short int errorcode;
	    switch(opcode)		
	    {
	     case 1: //readreq //open file
		     //printf("INside ReadRequest file name is %s\n",extrainfo);

		     file = fopen(extrainfo, "rb");
		     if (!file)
		      {
                        if(errno == EACCES)
			 {
                          errorcode = 2;
                          errstring="Access Violation";
			 }
                        else if(errno == ENOENT)
			 {
                          errorcode = 1;
			  errstring="File Not Found";
			 }
                  
			buflen = MakeErrorPacket(5,errorcode,errstring,&sendbuf);
		    	z = sendto(s,/* Socket to send result */sendbuf,buflen, /* The datagram lngth */
				0,/* Flags: no options */(struct sockaddr *)&adr_clnt,/* addr */len_inet); /* Client address length */
	   		if ( z < 0 )
	        	  bail("sendto(2)");
                        break;
		      }	
		     //Get file length
		     fseek(file, 0, SEEK_END);
	       	     filelen=ftell(file);
	             fseek(file, 0, SEEK_SET);
		     //update client structure 
                     newfd = MAXPORT+(inc++);

		     ss = socket(AF_INET,SOCK_DGRAM,0);
		     if ( ss == -1 )
		     bail(" new socket()");
		     printf("Value of new sock on creation is %d\n",ss);
		     /*
		     * Create a socket address, for use
		     * with bind(2):
		     */
		     memset(&adr_inet,0,sizeof adr_inet);
		     adr_inet.sin_family = AF_INET;
		     adr_inet.sin_port = htons(newfd);
		     adr_inet.sin_addr.s_addr =
		     inet_addr(srvr_addr);
		     if ( adr_inet.sin_addr.s_addr == INADDR_NONE )
			bail("bad address.");
		     len_inet = sizeof adr_inet;
		     /*
		     * Bind a address to our socket, so that
		     * client programs can contact this
		     * server:
		     */
		     z = bind(ss,(struct sockaddr *)&adr_inet,len_inet);
		     if ( z == -1 )
			bail("bind()");
		     //printf("Bind sucess to NEW  port %d\n ",ss);

		     add_Client(file,block,filelen,adr_clnt,ss,0,NULL); // add client detail to clients list

                     printf("CLient is added now\n");
	  	     FD_SET(ss,&master_list); //ADD new client to readfs file
          	     if(ss>fdmax) fdmax=ss;  //set it to max
		     printf("before readfile\n");
		     nbytes = readfile(file,block,&sendbuf);
                     //int i;
            	     //for(i=0;i<nbytes;i++)
            	     //printf("byte[%d] %x \n",i,(sendbuf)[i]);
                     n =MakeDataPacket(3,1,sendbuf,&buffer,nbytes);
		     printf("nbytes send to here is %d and buffer is %s\n",nbytes,buffer);
                     //int i;
            		//for(i=0;i<n;i++)
            		//printf("byte[%d] %x \n",i,(buffer)[i]);
		     z = sendto(ss,/* Socket to send result */buffer, n, /* The datagram lngth */
				0,/* Flags: no options */(struct sockaddr *)&adr_clnt,/* addr */len_inet); /* Client address length */
	    	     if ( z < 0 )
	        	bail("sendto(2)");
  		     //now read 512 bytes from a filename at 0 block and sendto first 512 bytes to client here
                     printf("Value of z in read req is %d\n",z);
                     if(z<512)
		     {
                      printf("File Copied Successfully to Socket :%d\n",ss);
		      close(ss);
                      FD_CLR(ss,&master_list);
		     }      
		    break;

	     case 4 ://ack received, check for duplicate ack
		     file = search_client(i);
                     printf("Socket is %d\n",i);
                     blockno = atoi(extrainfo);
                     lastblockread =getlastblockno(i);
		     printf("Inside ACK Received for blockno %d and last blockread is %d\n",blockno,lastblockread);
                     if(lastblockread == blockno -1 )
                      {
			//send error stating that duplicate ack
		        printf("Get Dulicate Ack.Drop the Packet and Continue\n");
		      }
                     updatetimetozero(i,true);
		     nbytes = readfile(file,blockno+1,&sendbuf);
                     setblockno(i,blockno+1);
                     n = MakeDataPacket(3,blockno+1,sendbuf,&buffer,nbytes);
		     z = sendto(i,/* Socket to send result */buffer, n, /* The datagram lngth */
				0,/* Flags: no options */(struct sockaddr *)&adr_clnt,/* addr */len_inet); /* Client address length */
	    	     if ( z < 0 )
	        	bail("sendto(2)");
		     //printf("z is %d\n",z);
                     if(z<512)
		     {
		     printf("File Copied Successfully to Socket and clearing i :%d\n",i);
		     FD_CLR(i,&master_list);
		     close(i);
		     }
                     /*printf("Master list after completely sendding packet is : ");
                     for(i=0;i<FD_SETSIZE;i++)
                      {
                        if (FD_ISSET(i, &master_list))
			 printf(" %d ",i);
		      }*/
		    break;

	     case 5 ://errorcode
		     printf("Beefore send error\n");
                     printf("In case 5 extrainfo here is %s\n",extrainfo);
		     z = MakeErrorPacket(5,0,extrainfo,&sendbuf);//5=opcode,0=for unknown error,extrainfo=errstring,dgram=sendto client
                     //printf("Value of send buf is %s\n",sendbuf);
                    
		     dgram[z] = 0;/* null terminate */
		     z = sendto(s,/* Socket to send result */sendbuf,z, /* The datagram lngth */
				0,/* Flags: no options */(struct sockaddr *)&adr_clnt,/* addr */len_inet); /* Client address length */
	  	     if ( z < 0 )
	        	bail("sendto(2)");
		     break;
		
	     }//end of switch
	   } //end of if fdset
         }//end of looping in fds
       }//end of else for retval
     }//end of for(;;)  
close(s);
return 0;
}

