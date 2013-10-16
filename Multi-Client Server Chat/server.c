#if 0
Authors:
Ankit Gupta MSCS Fall 2012 UIN:621009649
Osama Arshad UIN:315004499
Date:18 Sept 2012
#endif
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SBCPHeader.h"

#define STDIN 0
#define PORT 1234

#define STDIN 0
#define STDOUT 1
#define VRSN 3

Clients *head=NULL;

//returns 0 if not found else -1 for found
int search_username(char *username)
{
 //printf("username in searching function  is   %s\n",username);
 if(head==NULL) {return 0;} //list is empty not found
 Clients * temp=head;
 while(temp!=NULL)
 { 
  //printf("temp->username is %s\n",temp->username);
  if(!strcmp(temp->username,username))
  {  
   //printf("Username Exists here in clienfinfo.c\n");
   return -1;
  }
  else
   temp=temp->next;
 }
 return 0;
}

void print_clients()
{
 Clients *temp=head;
 while(temp!=NULL)
  {
   printf("[%d],[%s],[%d]\n",temp->clientid,temp->username,temp->idle);
   temp=temp->next;
  }
}

//returns -1 for not adding client
int add_client(int socketfd,char *username,bool idle)
{
 //printf("username and sockd in add_client functon is %d  %s\n",socketfd,username);
 Clients * temp=head;
 int found = search_username(username);
 //printf("Values of found is %d\n",found);
 if(found!=-1)//means not found have to add
 {
  Clients *newclient = (Clients*)malloc(sizeof(Clients));
  if(newclient==NULL)
  {
   printf("Error in Allocating Memory to new client\n");
   return -1;
  }
  newclient->username=username;
  newclient->clientid=socketfd;
  newclient->idle=false;
  newclient->next=NULL;
  Clients * temp=head;
  if(temp==NULL) //first element in a list to be added
  { 
   head=newclient;
  }
  else //put element at last
  {
   while(temp->next!=NULL)
	temp=temp->next;
   newclient->next=NULL;
   temp->next=newclient;
  }
 }
 else
 {
  //printf("\nDuplicate Name found\n");
  return -1;
 }
return 0;
}


void username_resolution(int pid,char **name)
{
 Clients * temp=head;
 while(temp!=NULL)
 { 
  //printf("head is not null \n");
  if(temp->clientid==pid)
  {
    //printf("In loop for checking for name\n");
    //printf("Username after resolution is : %s\n",temp->username);
    *name=(char*)malloc(strlen(temp->username));
    memset(*name,0,strlen(temp->username));
    memcpy(*name,temp->username,strlen(temp->username));
    //*name[strlen(temp->username)] = '\0';
    //printf("output after resolution %s\n",*name);
    return;
  }
  else
  {
   temp=temp->next;
   //printf("Next element \n"); 
  }
 }
return;
}


void set_status(int pid,bool status)
{
 Clients *temp=head;
 while(temp!=NULL)
 {
  if(temp->clientid==pid)
    {
     temp->idle=status;break;
    }
  else
   temp=temp->next;
 }
}



void clientlist(char **masterlist)
 {
  Clients *temp=head;
  *masterlist=(char*)malloc(256);
  bzero(*masterlist,256);
  int newl=0,len;
  char *name;
  while(temp!=NULL)
  {     
   name=temp->username;
   len =strlen(name);
   memcpy(*masterlist+newl,name,len);
   memcpy(*masterlist+newl+len," ",1);
   printf("\n");
   newl+=len+1;
   temp=temp->next;
  }
  //memcpy(*masterlist+newl-1,"0",1)
 //printf("in clientlist futn %s\n",*masterlist);
 return ;
}

int delete_client(int pid)
{
 Clients *temp, *m;
 temp=head;
 while(temp!=NULL)
 {
  if(temp->clientid==pid)
  {  
    if(temp==head)
    {
     head=temp->next;
     free(temp);
     return 0;
    }
    else
    {
    m->next=temp->next;
    free(temp);
    return 0;
    }
  }
  else
  {
   m=temp;
   temp= temp->next;
  }
 }//end of while
 //printf(" ELEMENT %d NOT FOUND\n ",pid);
return -1;
}




int MakePacket(short int mtype,short int attype,char **buf2,char* extrainfo)
{       

	//printf("#############in MakePacket Server extrainfo is %s and len is %d\n",extrainfo,strlen(extrainfo));
        SBCPMsg msga;
	SBCPAttr atta;
        msga.vrsntype = VRSN<<7 | mtype;
	short int msglen = 4+4+strlen(extrainfo);
	msga.mlength   = msglen; 
        atta.type     = attype;
        atta.alength  = 4+strlen(extrainfo);
        //data copy start
        *buf2=(char*)malloc(msglen);
        memcpy(*buf2,&msga,4);
        memcpy(*buf2+4,&atta,4);
        memcpy(*buf2+8,extrainfo,strlen(extrainfo));
       #if 0 //printf(" [Make packet] = [%s] \n",*(buf2));
        /*
        short int temp=VRSN<<7 | mtype;
	short int msgl = 4+4+strlen(extrainfo);
	short int attl = 4+strlen(extrainfo);
	//printf("Extrainfo is  %s, msgl is %d, attle is %d\n",extrainfo,msgl,attl);
	*buf2 =(char*)malloc(msgl);
	memset(*buf2,0,msgl);
	short int *x= &temp;
	memcpy(*buf2,x,2); //vrsntype
	x = &msgl;
	memcpy(*buf2+2,&msgl,2);
	x = &attype;		
	memcpy(*buf2+4,&attype,2);
	x = &attl;
	memcpy(*buf2+6,&attl,2);
	memcpy(*buf2+8,extrainfo,strlen(extrainfo));
        */  
        // printf("bytes in MAKING PACKET in server\n");
	//int k;
        //for(k=0;k<msgl;k++)	
	//printf("byte[%d] : %x\n",k,(*buf2)[k]);
        //printf("\n");
      #endif
   return msglen;
}

int ExtractPacket(char *buf1,char **output)
{
	//printf("inside Extract packet \n"); 
	SBCPMsg* MsgPtr =  buf1;
	SBCPAttr* attr =  (buf1+4);
	short int x=MsgPtr->vrsntype;
	short int mtype=127 & x;
        short int version= (x>>7)&1023;
	//short int msglen =  MsgPtr->mlength;
        //printf("mtype is %d\n",mtype);
        //int k =0; 
	//for(k=0;k<msglen;k++)	
	//printf("byte[%d] : %x\n",k,buf1[k]);
 	if(mtype<2 || mtype >9)
	  {
	   //printf("Unrecognized Data \n");
	   return -1;
	  }
        else if(version != VRSN)
	 {
	  #if 0 /*mtype=5;//set mtype as NAK
           char *reason="VERSION MISMATCH.CLOSING CONNECTION\n";
           printf("reason len  is %d\n",strlen(reason));
           *output =(char*)malloc(strlen(reason));
           memset(*output,0,(strlen(reason)));
	   memcpy(*output,reason,(strlen(reason)));*/
	#endif
	   printf("Version Type Mismatch\n");
	   return -1;
         }
        else
         {
	 //printf("version is correct. now try to parse \n");
	 //printf("attr->alength = [%d]",attr->alength);
	 *output =(char*)malloc((attr->alength)-4);
         memset(*output,0,(attr->alength)-4);
	 memcpy(*output,buf1+8,(attr->alength)-4);
         //printf("output  is %s\n",*output);
         }
	 return mtype;
}

void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
 if (argc < 4) {
   fprintf(stderr,"usage %s SERVER_IP PORT_NAME MAX_CLIENTS\n", argv[0]);
   exit(0);
  } 
 printf("\n**************Welcome To ClientChat Server************\n\n");
 int maxClients = atoi(argv[3]);
 fd_set master_list;
 fd_set read_fds;
 int fdmax,sockfd,newfd,nbytes;
 // listening socket descriptor
 struct sockaddr_storage remoteaddr; // client address
 socklen_t addrlen;
 char buffer[512];
 // buffer for client data
 char remoteIP[INET6_ADDRSTRLEN];
 int yes=1,i=0, j, rv,clientcount = 0;
 // for setsockopt() SO_REUSEADDR, below
 struct addrinfo hints, *ai, *p;
 FD_ZERO(&master_list);
 FD_ZERO(&read_fds);
 // clear the master_list and temp sets
 // get us a socket and bind it
 memset(&hints, 0, sizeof hints);
 hints.ai_family = AF_INET;
 hints.ai_socktype = SOCK_STREAM;
 hints.ai_flags = AI_PASSIVE;
 if ((rv = getaddrinfo(argv[1], argv[2], &hints, &ai)) != 0) {
  fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
  exit(1);
 }
 for(p = ai; p != NULL; p = p->ai_next) 
 {
   sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
   if (sockfd < 0) {
      continue;
      }
  // lose the pesky "address already in use" error message
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  if (bind(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
      close(sockfd);
      continue;
      }
     break;
  }
 // if we got here, it means we didn't get bound
 if (p == NULL) {
   fprintf(stderr, "selectserver: failed to bind\n");
   exit(2);
  } 
  freeaddrinfo(ai); // all done with this
  // listen
  if (listen(sockfd, 10) == -1) 
   {
    perror("listen\n");
    exit(3);
   }
 // add the sockfd to the master_list set
 FD_SET(sockfd, &master_list);
 fdmax = sockfd; // so far, it's this one

 while (1) 
   { 
     read_fds = master_list; // copy it
     //printf("before select\n");
     if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) 
     {
            perror("error in select\n");
            exit(4);
     }
     //printf("select returns\n");
     for(i=0;i<=fdmax;i++)
      {
       if (FD_ISSET(i, &read_fds))
      	{
	 if(i==sockfd)
	   {     
		addrlen = sizeof remoteaddr;
       	  	newfd = accept(sockfd,(struct sockaddr *)&remoteaddr,&addrlen);
	        if (newfd < 0)  error("ERROR on accepn\n");
	  	FD_SET(newfd,&master_list); //ADD new client to readfs file
          	if(newfd>fdmax) fdmax=newfd;  //set it to max
         	/* printf("selectserver: new connection from %s on "
				"socket %d\n",
				inet_ntop(remoteaddr.ss_family,get_in_addr((struct sockaddr*)&remoteaddr),
				remoteIP, INET6_ADDRSTRLEN),newfd);*/
		clientcount++;
                if(clientcount > maxClients)
		{  
		 //printf("Maximum number of client reached.Closing Connection\n");
		 short int typenak = 5; //for NAK MSG
	         char *buf=NULL;
                 short int attype = 1; //attribute type=1 for reason
	         char * reason = "Maximum number of clients reached\n";   
	         int n=MakePacket(typenak,attype,&buf,reason);
	         if (send(newfd, buf, n, 0) == -1) perror("error in sending NAK MSG\n");
		 close(newfd);
                 FD_CLR(newfd,&master_list);
		 clientcount--;
		 //break;	
		}
            }
	   else // handle data from a client
            {
               if ((nbytes = recv(i, buffer, 512, 0)) <= 0) //client hungup
		{ 
                   if (nbytes == 0) //client leaves the chat session.have to inform all other clients about its departure
		    {
 			short int typeoff =6; //set msg type as OFFLINE
	  	        char *buf=NULL;
                        char *uname=NULL;
		        short int attype = 2; //attribute type=2 for username
  		        //Update masterList and delete that client        
 		        username_resolution(i,&uname);
			if(delete_client(i)== -1)
			{
			 printf("\nUnknown Client Exited\n");
			 close(i);
                         FD_CLR(i,&master_list);
			break;
			}
			 int k=MakePacket(typeoff,attype,&buf,uname);
			  for(j = 0; j <= fdmax; j++) 	// send to everyone!
		          {   
                            if (FD_ISSET(j, &master_list)) // except the server socket(sockfd) and itself(i)
	                     { 
				if (j != sockfd && j != i) 
				  { 
				    if (send(j, buf, k, 0) == -1) perror("error in sending OFFLINE MSG\n");
					printf("%s is offline now !!\n",uname);
				   }  
                              }
                           }
                         //free(buf);
		    } 
		   else {
                            perror("error in receving bytes\n");
                        }
		   
                    close(i); // Free all the resource of this client by closing its socket
                    clientcount--;
                    FD_CLR(i, &master_list); // remove from master set
                 } //end of if client hungup

	        else // we got some data from a client
		{ 
			  //printf("Recvd Data \n");
			  char* payload=NULL;//this contains name,msg
			  short int type = ExtractPacket(buffer,&payload);
			  short int attype,typeidle,typefwd,typeack,typeonline,typenak;
                 
			  char *buf=NULL;int n;
			  char *username=NULL;
			  char *reason=NULL;
			  //printf("type = [%d] \n",type);
			  //Clients masterList;
		    	  //read buffer INTO sbcpPacket and check for type of packet(JOIN=2,SEND=4,IDLE=9)
			   switch(type)
			   { 
			    case 5: //NAK MSG    
					typenak = 5; //for NAK MSG
                                        attype = 1; //attribute type=1 for reason
	                	      	reason =(char*)malloc(strlen(payload)+1);
		 			memset(reason,0,strlen(payload)+1);
			                memcpy(reason,payload,strlen(payload)+1);
			                n=MakePacket(typenak,attype,&buf,reason);
				        if (send(i, buf, n, 0) == -1) perror("error in sending NAK MSG\n");
			                close(i);
					clientcount--;
					FD_CLR(i,&master_list);
					break;
				     
			    case 2:    
					#if 0//JOIN MSG,CHK FOR USERNAME and ADD IN LIST if exists SEND NAK else NAK
		 	 	        /*if(search_username(username)) //sends NAK request to client if return 1 means found
			   	        {
			    		 short int typenak = 5; //for NAK MSG
					 char *buf=NULL;
                                         short int attype = 1; //attribute type=1 for reason
	                	         char * reason = "Username exists\n";   
			                 n=MakePacket(typenak,attype,&buf,reason);
				         if (send(i, buf, n, 0) == -1) perror("error in sending NAK MSG\n");
			                 close(i);
					 break;
				        }
			                else
			    	        {*/	
				        //ADD NAMES to MASTERLIST Clients 
				        #endif
					username =(char*)malloc(strlen(payload)+1);
	        			memset(username,0,strlen(payload)+1);
					memcpy(username,payload,strlen(payload)+1);
					//printf("\nJOIN MSG REQUEST RECEIVED from %s\n",username);
					int exist =add_client(i,username,false);    
			                if(exist==-1) //means add_client failed.there is already username
				         {
					  short int typenak = 5; //for NAK MSG
					  char *buf=NULL;
                                          short int attype = 1; //attribute type=1 for reason
	                	          char * reason = "Username exists\n";   
			                  n=MakePacket(typenak,attype,&buf,reason);
				          if (send(i, buf, n, 0) == -1) perror("error in sending NAK MSG\n");
			                  close(i);
					  FD_CLR(i,&master_list);
					  clientcount--;
					  break;
					 }
				        printf("New JOIN Request from %s on socket %d\nUsername:%s\n",inet_ntop                  						(remoteaddr.ss_family,get_in_addr((struct sockaddr*)&remoteaddr),remoteIP, 						INET6_ADDRSTRLEN),newfd,username);
					printf("SENDING ACK MSG TO : %s\n",username);

			    	        typeack = 7 ;    //ACK MSG=7;
				        attype = 4; //attribute type=4 for message,contains client count                								//masterlist(clientcount,user1,user2,user3.
					typeonline=8;//for online
			                buf=NULL;
				        char *countlist=NULL;
				       
				        char *masterlist=NULL;
				        clientlist(&masterlist);
					//printf("here masterlist is  %s \n",masterlist);
					int p=strlen(masterlist)+4+4;
					//printf("length to be allocated before ack is  %d\n",p);
				        countlist =(char*)malloc(p); //+1 for , concating both
					bzero(countlist,p);
					sprintf(countlist,"%d : %s",clientcount,masterlist);
					//printf("here countlist is  %s \n",countlist);
                                        //printf("List of online users : %s \n",masterlist);
					
					 for(j = 0; j <= fdmax; j++) 	// send to everyone!
	          			  {   
                  			   if (FD_ISSET(j, &master_list)) // except the server socket(sockfd) and itself(i)
		  			    {      
					      if(i==j)
				    	      {	
					        int len = MakePacket(typeack,attype,&buf,countlist); 			  
				       		if (send(j, buf,len, 0) == -1) perror("error in sending ACK/client MSG\n");
  						    //printf("ACK MSG SEND\n");
				     	      }  //end if(i==j)

					     if ((j != sockfd) && (j != i)) //ALSO SEND ONLINE MSG TO ALL ARRIVING OF THIS CLIENT
					      {
 						
						attype=2;//username
						n=MakePacket(typeonline,attype,&buf,username);
						if (send(j, buf,n, 0) == -1) perror("error in sending ONLINE MSG\n");
	  						//printf("ONLINE MSG SEND\n");
					      }	
                                            
				            }
					  }
						
 					 //free(username);//free(buf);
		   		   break;

			   case 4: //Receive msg from client and have to send to all Clients
				    typefwd = 3 ;    //SEND MSG=4;
			            buf=NULL; 
				    attype = 4; //attribute type=4 for message     
				    char* uname=NULL;
				    username_resolution(i,&uname);
				    
				    char * chatc=NULL;
				    chatc=(char*)malloc(strlen(uname)+strlen(payload)+4);
				    memset(chatc,0,strlen(uname)+strlen(payload)+4);
				    sprintf(chatc,"[%s] : %s",uname,payload);
				    //char* chat=(char*)malloc(strlen(uname)+strlen(payload)+4);
				    //bzero(chat,strlen(uname)+strlen(payload)+4);	
			            n =MakePacket(typefwd,attype,&buf,chatc);//ACK msg with list of online client to i 
		                    for(j = 0; j <= fdmax; j++) 	// send to everyone!
          			    {   
          			     if (FD_ISSET(j, &master_list)) // except the server socket(sockfd) and itself(i)
	  			     { 	               			      
				      if (j != sockfd && j != i) 
			    	       {		  
			       		if (send(j, buf,n, 0) == -1) perror("error in sending CHAT MSG\n");
					    //printf("CHAT MSG  SEND\n");
			       	       }  
			             }
				    }	
				    //free(buf);		
				    break;
                     
			   case 9: //IDLE MSG received,server sends to all clients with this username that it is now idle
				   typeidle =9; //set msg type as IDLE
	  			   buf=NULL;
				   attype = 2; //attribute type=2 for username
				   //Update masterList         
				   set_status(i,true);
			           username_resolution(i,&username);
			           int len = MakePacket(typeidle,attype,&buf,payload);
				   //printf("Packet done \n");
				   for(j = 0; j <= fdmax; j++) 	// send to everyone!
	          	            {   
                  	             if (FD_ISSET(j, &master_list)) // except the server socket(sockfd) and itself(i)
		  		      { 	               
				   
					  if (j != sockfd && j != i) 
				    	 {		  
				          if (send(j, buf,len, 0) == -1) perror("error in sending IDLE MSG\n");
  					
						  printf("%s is Idle now !!\n",username);
						  //exit(0); // remove it later
				   
				       	 } 
				      }
			            }			
				     //free(username);//free(buf);
				     break;
	
			   default:  //if server receive any other type, it simply replies to client dont send
				     //printf("Unknown Header Packet.\n");
			   break;
			  		
    		 	 }//end of switch
		// free(payload);
             } //end handle data from client
	   
           } //end getting new connections
         } //end of for loop for each file descriptor for(int i=0;i<=fdmax;i++)
       } //end of while
     }
     close(sockfd);
     return 0; 
}

