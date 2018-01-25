/* ------------------------------------------------------------------------- */
/*   Copyright (C) 2017
                Author:  salyousefi2015@my.fit.edu
			rbomalaski2015@my.fit.edu
                Florida Tech, Human Decision Support Systems Laboratory

       This program is free software; you can redistribute it and/or modify
       it under the terms of the GNU Affero General Public License as published by
       the Free Software Foundation; either the current version of the License, or
       (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU Affero General Public License
      along with this program; if not, write to the Free Software
      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.              */
/* ------------------------------------------------------------------------- */
#include <time.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netdb.h> 
#include <unistd.h>
#include <time.h> ///* time_t, struct tm, difftime, time, mktime */
#include <errno.h>
#include "sha256.h"//hashing function.h
#include <arpa/inet.h>
#include <libtasn1.h>


#define BUFF 1024

    struct clientThreadParams {
      char *_host;
      char *_port;
      char *_message;
      char *_timestamp;
      int _is_udp;
    };


void *do_client(void *args){
  struct clientThreadParams *params = args;
  char* host = params->_host;
  char* port = params->_port;
  char* message = params->_message;
  char* timestamp = params->_timestamp;
  int is_udp = params ->_is_udp;
  
  //refresh this code
  
}

char* getTime(){
  time_t timestmp;
  timestmp=time(NULL);
  //get time
  char* CurrentTime =ctime(&timestmp);

  //get rid of invalid characters
  int i;
    for (i=0; CurrentTime[i]!= '\0'; i++)
    {
        if (CurrentTime[i] == ' ')
        {
            CurrentTime[i]='-';
        }
        if(CurrentTime[i]==':'){
	  CurrentTime[i] = '-';
	}
	if(CurrentTime[i] =='\n'){
	  CurrentTime[i] = '\0';
	}
    }
    //debug stuff
    //printf("Current Time is: %s\n", CurrentTime);
    //printf("Current time length  is: %lu\n", strlen(CurrentTime));
  return CurrentTime;
}


//udp
void UDP_client(char* theMessage, string HostName, int portNumber){
	char*  timestp,hashcmd;
	struct hostent *server;
	struct sockaddr_in serverAddr;
	char *databuff[BUFF];
	char *temp_mesg[BUFF];
	char hostname[BUFF]; 
	int clientSocket, nBytes,msglen,T;
	socklen_t addr_size;

  /*Create UDP socket*/
  clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

  /*Configure settings in address struct*/
  strcpy(hostname, HostName);
  server = (struct hostent *) gethostbyname(hostname);
  bzero((char *) &serverAddr, sizeof(serverAddr));
  bcopy((char *)server->h_addr, (char *)&serverAddr.sin_addr.s_addr, server->h_length);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(portNumber);
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*Initialize size variable to be used later on*/
  addr_size = sizeof serverAddr;

  while(1){
    if(theMessage.compare("")!=0){
      if(theMessage.compare("quit")==0){
	exit(1);}
	else if(theMessage.compare("leave")==0){
	//send leave command
      }else if (theMessage.compare("help")==0){
	//help
	printf("Enter a message to send a message.\n");
	printf("Type quit to quit.\n");
	theMessage ="";
	hashcmd = "";
      }else{
	char* hashed = sha256(theMessage);
	timestp = getTime();
	hashcmd="GOSSIP:"+hashed+":"+timestp+":"+theMessage+"%\n";
	//printf("UDP - Message is: %s\n",hashcmd.c_str());
	
	
	//encode the client message 
	int x;
	strcpy(temp_mesg, hashcmd);
	x = EncodeMsg(temp_mesg, databuff);
	
	//Send message to server
	msglen = sendto(clientSocket,databuff,x,0,(struct sockaddr *)&serverAddr,addr_size);
	
	vector<string> list = parseCommand(theMessage);
			if(list.at( 0 ).compare("PeersQuery") == 0){
				T1 = 1 ;
			}
			
	// get the server reply and decode it
	nBytes = recvfrom(clientSocket,&databuff,BUFF,0,NULL, NULL);
	DecodeMsg(databuff, nBytes, T1);
	//Receive message from server
	
	if(nBytes>0){
	  printf("Message sent\n");
	}
	theMessage = "";
	hashed = "";
      }
    }else{
      //get message
      //prompt for message
	    printf("UDP:Please Enter a command or help: \n ");
	    getline(cin,theMessage);
    }
  }
}

//tcp
void TCP_client(char* theMessage, char* HostName, int portNumber){
	// how can I convert from string temp_mesg to pointer char*database
	char*  timestp,hashcmd;
	struct hostent *server;
	struct sockaddr_in serverAddr;
	int sock,sockfd, msglen,T;
	char *databuff[BUFF];
	char *temp_mesg[BUFF];
	socklen_t leg;
	char hostname[BUFF]; 
	// create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1)
	{
	 cout<<" ERROR: unable to create the socket"<<"\n";
	 exit(0);
  
	}
    
       // get the host by name
    	strcpy(hostname, HostName);
    	server = (struct hostent *) gethostbyname(hostname);
    	if (server == NULL) {
        	fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        	exit(0);
    	}
    	
	//connect to the server
    
	
    	bzero((char *) &serverAddr, sizeof(serverAddr));
        bcopy((char *)server->h_addr, (char *)&serverAddr.sin_addr.s_addr, server->h_length);

     	serverAddr.sin_family = AF_INET;
    	serverAddr.sin_port = htons(portNumber);
    	
    	//connect 
	if(connect(sock,(struct sockaddr*)&serverAddr, sizeof(serverAddr))==-1){
		fprintf(stderr, "\n ERROR can not connect to the server ..\n");
	exit(0);
	}

	// If the user didn't start with a message at command line, say hello and prompt for message
	if (theMessage.compare("")==0){
		
		printf("Hello, Welcome to my peers client..\n");
	
	
	}
	// This will listen for messages 1 at a time
	// There are two invalide messages: "quit" and "help" which are reserved for client funcationality
	// quit will quit the program
	// help will display a help dialog
	// All other input is used as a message and sent to the server declared when launching.
	leg = sizeof(serverAddr);
	while(1){
	   if (theMessage.compare("")!=0){
	     if(theMessage.compare("quit")==0){
	      
		exit(0);}
	      else if(theMessage.compare("leave")==0){
		//send leave command
			
	      }
		  else if(theMessage.compare("help")==0){
		  printf("Enter a message to send a message.\n");
	      printf("Type quit to quit.\n");
		  theMessage = "";
		  continue;
			
	      }
		//hash the message with SHA256///
	      char* hashed =sha256(theMessage);
	      //get the current time ///
	      timestp = getTime();
	      //concat the whole message
	      hashcmd="GOSSIP:"+hashed+":"+timestp+":"+theMessage+"%\n";
	      //debug for displaying message
	      //printf("TCP - Message is: %s\n",hashcmd.c_str());
	      //printf("Size is %lu\n",strlen(hashcmd.c_str()));
			
		  //encode the client message 
		  int x;
		  strcpy(temp_mesg, hashcmd);
		  x = EncodeMsg(temp_mesg, databuff);
		   //send the message
	      write(sock,&databuff,x);
	      
		  vector<string> list = parseCommand(theMessage);
			if(list.at( 0 ).compare("PeersQuery") == 0){
				T1 = 1 ;
			}
			 // get the server reply and decode it
	      msglen= read(sock,&databuff, BUFF);
		  DecodeMsg(databuff, msglen, T1);

	      if (msglen>0){
		  printf("command sent \n"); 
		  }
	      //set message and hashcmd to null, so that we don't continually send them
	      theMessage = "";
	      hashcmd = "";
			
	    }else{
			printf("TCP:Please Enter a message or help: \n ");
			getline(cin,theMessage);
		}

	}
}



//sha encoder
  /*
   * Needs to be corrected
   */