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
/* 
 * This is our server. code.  This runs the program as a server.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <pthread.h>
#include <sqlite3.h>
#include "asn_translate.h"
#include "server.h"

#define MAX_CALLS 5
#define MAXLINE 1024



// Structs are used by multiple functions, so struct is declared outside of a function
struct serverListenTParams {
    int server_port;
    char *database;
    int *_timeout;
  };
  
struct tcp_stuff{
    int _msglen;
    int _cli_socket;
    char *_buffer;
    char *_returnMessage;
    char *_dbfilepath;
      };
      
struct udp_stuff {
    int _sockfd;
    int _client_addr_len;
    struct hostent *_hostp;
    struct sockaddr_in _cli_addr;
    char *_hostaddrp;
    char *_dbfilepath;
    char *_buffer;
    int _msglen;
  };

// concat is for contatenating strings.  I very much dislike how C handles strings, and I find it
// quite confusing.  
char* concat( const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

// used later
static int callback(void *count, int argc, char **argv, char **azColName){
   int i;
   int j= 0;
   char string[256],c;
   for(i=0; i<argc; i++){
     printf("arv[%d] = %s\n",i,argv[i]);
     concat(string,("%s",argv[i]));
   }
   printf("%s\n",string);
   return 0;
}


// Main Server Function
//  This takes in a struct of parameters from the main, and then opens
//  a thread for both UDP and TCP.  These threads use the struct to 
//  get the values they need to run.
void *do_server(void *args){
  printf("Server time!\n");
  //grab args
  // this is the struct passed to do_server
  struct serverThreadParams {
    char *port;
    char *database;
  };
  
  struct serverThreadParams *params = args;
  char* port_char = params->port;
  char* db_add = params->database;
  sqlite3 *db;
  char *sql_create_tables;
  char *zErrMsge = 0;
  int rc;
  
  
  if(port_char ==NULL){
    //exit no port
    printf("You did not include a port for the server.\n");
    exit(1);
  }
  
  if (db_add == NULL){
    //exit no db
    printf("You did not include a database address for the server.\n");
    exit(1);
  }
  
  int port = atoi(port_char);
  if (port>65535 || port <1){
    //exit invalid port num
    printf("You did not include a valide port for the server.\n");
    exit(1);
  }
  
  //create DB
  rc = sqlite3_open(db_add, &db);
  sql_create_tables = "CREATE TABLE PEERS("  \
         "NAME TEXT PRIMARY KEY    NOT NULL," \
         "IP            TEXT     NOT NULL," \
         "PORT        TEXT NOT NULL );" \
         "CREATE TABLE GOSSIP(" \
         "SHA256 TEXT PRIMARY KEY NOT NULL," \
         "DATE_TIME TEXT, " \
         "MESSAGE TEXT );";
  
  rc = sqlite3_exec(db,sql_create_tables,callback, 0, &zErrMsge);
  
  rc = sqlite3_close(db);
  
  
  // this is the struct of values being sent to the listening threads
  struct serverListenTParams *serverStuff;
  serverStuff = malloc(sizeof(*serverStuff));
  serverStuff->server_port = port;
  serverStuff->database = db_add;
  
    
  //Create thread for UDP and thread for TCP
  pthread_t tcp_thread;
  pthread_t udp_thread;
  pthread_create(&tcp_thread, NULL, listen_TCP,serverStuff);
  pthread_create(&udp_thread, NULL, listen_UDP,serverStuff);
    
  // don't exit while child threads are still listening
  pthread_exit(NULL);
}


// Function to listen on tcp
//    This function will listen on TCP, and create a new thread to process info when it receives what it needs

void *listen_TCP(void *args){
  struct sockaddr_in cli_addr, serv_addr;
  int serv_socket, cli_socket, clilen, iobuffer,msglen;
  char buffer[MAXLINE];
  char *returnMessage;
  
  struct serverListenTParams *tcpargs = args;
  int portNum = tcpargs->server_port;
  char* dbfilepath = tcpargs->database;
  
  
  if ((serv_socket = socket(AF_INET, SOCK_STREAM, 0))<0){
    perror("server can't open stream socket - tcp\n");
    exit(1);
  }
  
  serv_addr.sin_family        = AF_INET;
  serv_addr.sin_port          = htons(portNum);
  serv_addr.sin_addr.s_addr   = htonl(INADDR_ANY);
  
  socklen_t serv_addr_size = sizeof(serv_addr);
  
  if(bind(serv_socket, (struct sockaddr*) &serv_addr, sizeof(serv_addr))<0){
    perror("error binding - tcp\n");
    exit(1);
  }
  
  if(listen(serv_socket, MAX_CALLS)<0){
    perror("server failed to listen - tcp\n");
    exit(1);
  }
  
  socklen_t cli_addr_size = sizeof(cli_addr);
  while(1){
    cli_socket = accept (serv_socket, ( struct sockaddr*) &cli_addr, &cli_addr_size);
      //write (cli_socket, "Please enter your command\n", strlen("Please enter your command\n"));
 
    
    //struct to send to thread
      struct tcp_stuff *tcpStuff;
      tcpStuff = malloc(sizeof(*tcpStuff));
      tcpStuff->_msglen = msglen;
      tcpStuff->_cli_socket = cli_socket;
      tcpStuff->_buffer = buffer;
      tcpStuff->_returnMessage = returnMessage;
      tcpStuff->_dbfilepath = dbfilepath;
      
      pthread_t subServer;
      pthread_create(&subServer,NULL,tcp_thread_stuff,tcpStuff);
      //pthread_join
      //free(tcpStuff);
      }
  
}

void *tcp_thread_stuff(void *args){
        //write (cli_socket, "Please enter your command\n", strlen("Please enter your command\n"));

      struct tcp_stuff *relevantInfo = args;
      int msglen = relevantInfo->_msglen;
      int cli_socket = relevantInfo->_cli_socket;
      char *buffer = relevantInfo->_buffer;
      char *returnMessage = relevantInfo->_returnMessage;
      char *dbfilepath = relevantInfo->_dbfilepath;
  
      while( (msglen= recv(cli_socket, buffer, MAXLINE,0))>0){
	printf("Received Message: %s.\n",buffer);
	
	returnMessage = messageProcessor(buffer,dbfilepath,msglen);
	write(cli_socket, returnMessage, sizeof(returnMessage));
	memset(&buffer[0],0,MAXLINE);
      }
  
      //client close bit
      if (msglen == 0){
	printf("Client diconnected - TCP\n");
	fflush(stdout);
      }
  
      if (msglen<0){
	printf("Something happened!\n");
	free(args);
	exit(1);
      }
      free(args);
}

void *listen_UDP(void *args){
  
  struct serverListenTParams *UDPargs = args;
  int portNum = UDPargs->server_port;
  char* dbfilepath = UDPargs->database;
  
  int sockfd, optval, msgbytesize, client_addr_len;
  struct sockaddr_in serv_addr, cli_addr;
  struct hostent *hostp;
  char buffer[MAXLINE];
  char *hostaddrp;
  
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
    perror("server can't open datagram socket - udp\n");
  
  //build server internet address
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons((unsigned short)portNum);
  
  //bind
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    perror("error binding - udp\n");
  client_addr_len = sizeof(cli_addr);
  while(1){

    //recvfrom - UDP equivalent of recv
    bzero(buffer, MAXLINE);
    
    msgbytesize = recvfrom(sockfd, buffer, MAXLINE, 0,
		 (struct sockaddr *) &cli_addr, &client_addr_len);
    
    if (msgbytesize < 0)
      perror("ERROR in recvfrom");
    
    if(msgbytesize>0){
      

      struct udp_stuff *sendOver;
      sendOver = malloc(sizeof(*sendOver));
      sendOver->_sockfd = sockfd;
      sendOver->_client_addr_len = client_addr_len;
      sendOver->_cli_addr = cli_addr;
      sendOver->_hostp = hostp;
      sendOver->_hostaddrp = hostaddrp;
      sendOver->_dbfilepath = dbfilepath;
      sendOver->_buffer = buffer;
      sendOver->_msglen = msgbytesize;
      
      pthread_t udp_subserver;
      pthread_create(&udp_subserver,NULL,udp_thread_stuff,sendOver);
      //pthread_join;
      //free(sendOver);
      

    }
  }
  
  // Listen
  // Create Thread on receiving message
  // process that message in its own thread
  // kill that thread
}

void *udp_thread_stuff(void *args){
  char *returnMessage;
  int msgbytesize;
  

  
  struct udp_stuff *relevantStuff = args;
  int sockfd = relevantStuff->_sockfd;
  int client_addr_len = relevantStuff->_client_addr_len;
  struct hostent *hostp = relevantStuff->_hostp;
  struct sockaddr_in cli_addr = relevantStuff->_cli_addr;
  char *hostaddrp = relevantStuff->_hostaddrp;
  char *dbfilepath = relevantStuff->_dbfilepath;
  char *buffer = relevantStuff->_buffer;
  int msgln = relevantStuff->_msglen;

  
  
  
  hostp = gethostbyaddr((const char *)&cli_addr.sin_addr.s_addr, 
			  sizeof(cli_addr.sin_addr.s_addr), AF_INET);
  if (hostp == NULL)
    perror("ERROR on gethostbyaddr");
  hostaddrp = inet_ntoa(cli_addr.sin_addr);
  if (hostaddrp == NULL)
    perror("ERROR on inet_ntoa\n");
  printf("server received datagram from %s (%s)\n", hostp->h_name, hostaddrp);
    
    //echo input back to client, currently used for debugging
    
  returnMessage = messageProcessor(buffer,dbfilepath,msgln);
  msgbytesize = sendto(sockfd, returnMessage, strlen(returnMessage), 0, 
		       (struct sockaddr *) &cli_addr, client_addr_len);
  if (msgbytesize < 0) 
    perror("ERROR in sendto");
  free(args);
}

char * messageProcessor(char message[], char dbfilepath[], int msgln){
  int msglength;
  
  //call asn1 decode
  DecodeMsg(message,msgln,1);
  msglength = strlen(message);
  char firstLetter = message[0];
  char delimiter = ':';
  
  // check message commands
  // PLACEHOLDER METHOD, JUST CHECKING FIRST LETTERS!!!
  // THIS IS BAD PRACTICE, but is being used until I get everythign working
  // This will be corrected last, as we get to assume correct input for the actual assignment :)
  if(firstLetter=='G'){
    //DO GOSSIP
    return add_gossip(message,dbfilepath);
  }
  // peer or peers?
  else if(firstLetter =='P'){
    if (message[4]=='S'){
      //DO PEERS
      return get_peers(dbfilepath);
    }
    else{
      // DO PEER
      return add_peer(message,dbfilepath);
    }
  }
  // not a understandable command?
  else{
    return("Not a useable command.\n");
  }
}



//add peers
char * add_peer(char message[], char dbfilepath[]){
  char *name, *ip, *port;
  sqlite3 *db;
  char * i_hate, *how_c, *works_with, *strangs, *it_sucks;
  char *update, *update2, *update3, *update4, *update5;
  int rc;
  char *zErrMsge = 0;
  
  //parse out the bits we need from the message
  strtok(message,":=%");
  name = strtok(NULL,":=%");
  strtok(NULL,":=%");
  port = strtok(NULL,":=%");
  strtok(NULL,":=%");
  ip = strtok(NULL,":=%");
  
  //concatenate sql command
  i_hate = concat("INSERT INTO PEERS VALUES('",name);
  how_c = concat(i_hate,"','");
  works_with = concat(how_c,ip);
  strangs = concat(works_with,"',");
  it_sucks = concat(strangs, port);
  char *sql_insert;
  sql_insert = concat (it_sucks,");");
  
  //concatenate update command
  update = concat("UPDATE PEERS SET IP ='",ip);
  update2 = concat(update,"' ,PORT = ");
  update3 = concat(update2,port);
  update4 = concat(update3," WHERE NAME = '");
  update5 = concat(update4,name);
  char *sql_update;
  sql_update = concat(update5,"';");
  
  /*
   * ,port = ");
  update3 = concat(update2,port);
  update4 = concat(update3,"
   */
  
  //debug prints
  printf("This is your sequel command\n%s\n",sql_insert);
  printf("%s\n",name);
  printf("%s\n",port);
  printf("%s\n",ip);
  
  //open db
  rc = sqlite3_open(dbfilepath,&db);

  rc = sqlite3_exec(db,sql_insert,callback, 0, &zErrMsge);
  if( rc!=SQLITE_OK ){
    rc = sqlite3_exec(db,sql_update,callback, 0, &zErrMsge);
    
    //printf("This is your sequel command\n%s\n",sql_update);
    
    fprintf(stderr, "Updated.\n");
    sqlite3_free(zErrMsge);
    return ("Updated.\n");
  }

  
  //close db
  rc = sqlite3_close(db);
  return("Added\n");
}


//get peers
char * get_peers(char dbfilepath[]){
  char sql[200] = "SELECT * FROM PEERS";
  sqlite3 *db;
  int rc;
  char *zErrMsge = 0;
  
  rc = sqlite3_open(dbfilepath,&db);
  
  rc = sqlite3_exec(db,sql,callback, 0, &zErrMsge);
  if( rc!=SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsge);
    sqlite3_free(zErrMsge);
    }
  rc = sqlite3_close(db);
  
  return ("Got to get_peers\n");
}


// add gossip
char * add_gossip(char message[], char dbfilepath[]){
  char *sha256, *date_time, *text_mes;
  sqlite3 *db;
  char * i_hate, *how_c, *works_with, *strangs, *it_sucks;
  int rc;
  char *zErrMsge = 0;
  strtok(message,":");
  sha256 = strtok(NULL,":");
  date_time = strtok(NULL,":");
  text_mes = strtok(NULL,":%");
  
    //concatenate sql command
  i_hate = concat("INSERT INTO GOSSIP VALUES('",sha256);
  how_c = concat(i_hate,"','");
  works_with = concat(how_c,date_time);
  strangs = concat(works_with,"','");
  it_sucks = concat(strangs, text_mes);
  char *sql_insert;
  sql_insert = concat (it_sucks,"');");
  printf("got here3\n");
  
  //debug prints, remove later
  printf("This is your sequel command\n%s\n",sql_insert);
  printf("%s\n",sha256);
  printf("%s\n",date_time);
  printf("%s\n", text_mes);
  
   //open db
  rc = sqlite3_open(dbfilepath,&db);

  rc = sqlite3_exec(db,sql_insert,callback, 0, &zErrMsge);
  if( rc!=SQLITE_OK ){
    fprintf(stderr, "Discarded.\n");
    sqlite3_free(zErrMsge);
    return("Discarded.\n");
    }
  
  broadcastUDP(text_mes, dbfilepath);
  //close db
  rc = sqlite3_close(db);
  
  return ("Message Added!\n");
}


// Only broadcasting on UDP because of issues with lack of listeners on broadcastTCP
// If a client is not actively listening, then TCP will hang and be bad
// UDP doesn't care about listeners and will simply place a datagram to broadcast
// This means that a user could potentially miss a datagram if they aren't actively
// listening to a UDP port, but that's better than not functioning if they're
// not actively connected to a port
void broadcastUDP(char message[], char dbfilepath[]){
  int port;
  char* address, *peers;
//broadcast place filler
printf("Broad on UDP: %s\n",message);
// get peers
peers = get_peers(dbfilepath);
//for each peer
    //get port
    //get address
    //broadcasts on UDP 55440 for now, as a debug option
    send_gossip(55440,"127.0.0.1",message);
  
}

void send_gossip(int port, char* address, char* gossip_message){
	struct hostent *server;
	struct sockaddr_in serverAddr;
	char hostname[MAXLINE]; 
	int clientSocket, nBytes,msglen;
	socklen_t addr_size;

  /*Create UDP socket*/
  clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

  /*Configure settings in address struct*/
  strcpy(hostname, address);
  server = (struct hostent *) gethostbyname(hostname);
  bzero((char *) &serverAddr, sizeof(serverAddr));
  bcopy((char *)server->h_addr, (char *)&serverAddr.sin_addr.s_addr, server->h_length);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
  addr_size = sizeof serverAddr;
  
  nBytes = strlen(gossip_message) + 1;
  //Send message to server
  msglen = sendto(clientSocket,gossip_message,nBytes,0,(struct sockaddr *)&serverAddr,addr_size);
  if(msglen<0){
    printf("Issue\n");
  }

  
}