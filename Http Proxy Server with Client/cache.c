#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

typedef struct {
    //void *block;
    char *request;
    char *time;
    char *filename;
    bool isTimeExpire;
    struct cacheEntry *next;
} cacheEntry;


typedef cacheEntry * cacheptr;
cacheEntry *head=NULL;


int add_entry(char *addrequest,char *time,char *filename,bool isTimeExpire)
{
  //printf("###################Values to be added to entry is\n%s\n%s\n%s\n%d\n",addrequest,time,filename,isTimeExpire);
  cacheEntry *newentry = (cacheEntry*)malloc(sizeof(cacheEntry));
  if(newentry==NULL)
  {
   printf("Error in Allocating Memory to new entry\n");
   return -1;
  }
  newentry->request = (char*)malloc(strlen(addrequest));
  bzero(newentry->request, strlen(addrequest));
  strcpy(newentry->request,addrequest);
  
  newentry->time = (char*)malloc(strlen(time));
  bzero(newentry->time, strlen(time));
  strcpy(newentry->time,time);
   
  newentry->filename = (char*)malloc(strlen(filename));
  bzero(newentry->filename, strlen(filename));
  strcpy(newentry->filename,filename);
  
  newentry->isTimeExpire=isTimeExpire;
  newentry->next =(cacheptr)head;
  head=(cacheEntry*)newentry;
  printf("New Request is Added to Front\n");
  
 return 0;
}


//returns pointer to cacheEntry else if found else NULL for not found
struct cacheEntry * search_entry(char *requestrecv)
{
 if(head==NULL) {return NULL;} //list is empty not found
 cacheEntry * temp=head;
 while(temp!=NULL)
 { 
  if(!strncmp(temp->request,requestrecv,strlen(temp->request+1)))
  {  
   printf("Request Exist in Cache\n");
   return (cacheptr)temp;
  }
  else
   temp=(cacheEntry*)temp->next;
 }
 return NULL;
}

void print_cacheEntry()
{
 cacheEntry *temp=head;
 int i=1;
 while(temp!=NULL)
  {
   printf("===============Entry %d Start======================\n\n",i);
   printf("Request:\n%s\nTime:%s\nFilename:%s\nIsTimeExpire:%d\n",
   temp->request,temp->time,temp->filename,temp->isTimeExpire);
   //printf("[%d] Request is %s\n",i,temp->request);		
   i++;
   temp=(cacheEntry*)temp->next;
  }
}

/*
* Removes node from the last
*/

int delete_entry()
{
 cacheEntry *prev=head;
 cacheEntry *temp=(cacheEntry*)head->next;
 while(temp->next!=NULL)
 {
  temp=(cacheEntry*)temp->next;
  prev=(cacheEntry*)prev->next;
 }
  prev->next=NULL;
  printf("Cache Full \nDeleting Entry From last\n");
  free(temp);
  return 0;
}

int updateremoveaddtoHead(char *requestrecv,char *time,bool isTimeExpire)
{
 cacheEntry *temp, *prev;
 temp=head;
 
 while(temp!=NULL)
 {
   if(!strncmp(temp->request,requestrecv,strlen(temp->request)-2))
   {  
    //printf("Request Matches in Updateremoveandaddtohead \n");
    if(time)
    	temp->time = time;
    if(isTimeExpire)	
    	temp->isTimeExpire = isTimeExpire; 
    if(temp==head)
    {
    //printf("temp is head now\n");
    //do nothing keep it as a first entry 
    return 0;
    }
    else
    {
    //printf("if request is not headrequest\n");
    prev->next=temp->next;
    //add this temp to the front
 	cacheEntry *newentry = (cacheEntry*)malloc(sizeof(cacheEntry));
    if(newentry==NULL)
    {
      printf("Error in Allocating Memory to new entry\n");
      return -1;
    }
    newentry->request = (char*)malloc(strlen(temp->request));
    bzero(newentry->request, strlen(temp->request));
    strcpy(newentry->request,temp->request);
  
  	newentry->time = (char*)malloc(strlen(temp->time));
  	bzero(newentry->time, strlen(temp->time));
  	strcpy(newentry->time,temp->time);
  
  	newentry->filename = (char*)malloc(strlen(temp->filename));
  	bzero(newentry->filename, strlen(temp->filename));
  	strcpy(newentry->filename,temp->filename);
  
  	newentry->isTimeExpire=temp->isTimeExpire;
  	newentry->next = head; 
    head = newentry;
    free(temp);
    return 0;
    }
  }
  else
  {
   prev=temp;
   temp= (cacheEntry*)temp->next;
  }
 }//end of while
 //printf(" ELEMENT %d NOT FOUND\n ",pid);
return -1;

}

time_t get_time_sec(char * tame)
{
struct tm tm;
time_t t;
//printf("time format here in get_time_sec is::%s::\n",tame);
//char *tame = "Sat, 10 Nov 2012 16:41:51 GMT";
//char *tame = "Thu, 11 Oct 2012 07:15:35 GMT";
//printf("len of time in gettimesec %d\n",strlen(tame));
if (strptime(tame, "%a, %d %b %Y %H:%M:%S %Z", &tm) == NULL)
    /* Handle error */;

#if 0
printf("year: %d; month: %d; day: %d;\n",
        tm.tm_year, tm.tm_mon, tm.tm_mday);
printf("hour: %d; minute: %d; second: %d\n",
        tm.tm_hour, tm.tm_min, tm.tm_sec);
printf("week day: %d; year day: %d\n", tm.tm_wday, tm.tm_yday);
#endif

tm.tm_isdst = -1;      /* Not set by strptime(); tells mktime()
                          to determine whether daylight saving time
                          is in effect */
t = mktime(&tm);
if (t == -1)
    /* Handle error */;
printf("seconds since the Epoch: %ld\n", (long) t);

return t;
}

#if 0
int count = 0;
int main() {
 if(count < 3)
  {
  count++; 
  add_entry("Ankit Request","Monday","Cache",true);
  }
 if(count < 3)
  {
   count++;
   add_entry("Gupta Request","Friday","Namaste.txt",true);
  }
    
 cacheEntry * tmp = search_entry("Ankit Request");
 printf("tmp file is %s\n",tmp->filename);
  if(count < 3)
  {
  printf("Added\n");
  count++;
  add_entry("Atish Request","12-12-12","atish.txt",false);
  }
  print_cacheEntry();
  if(count < 4)
  {
   count++;add_entry("amruth Request","11-11-11","atish.txt",false);
  }
  printf("***********NOW DELETE *********************");
  delete_entry();
 print_cacheEntry();  
 printf("****************NOW DELETE AND ADD************************"); 
 updateremoveaddtoHead("Gupta Request","TIME X",false); 
print_cacheEntry();
    return 0;

}

#endif
