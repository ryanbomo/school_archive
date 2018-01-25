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
 * This is our main program.  This takes arguements and decides if we are a server
 * or a client and threads accordingly.
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
#include "server.h"
#include "client.h"

int
main (int argc, char **argv)
{
  char *pvalue = NULL; // port num
  char *dvalue = NULL; // database address
  char *hvalue = NULL; // host address
  char *mvalue = NULL; // message
  char *tvalue = NULL; // time stamp
  int time_out =0;// time out int in seconds
  int is_s = 0; // bool for if server
  int is_c = 0; // bool for if client
  int is_UDP = 0; // bool for if UDP, used to also check if TCP (assume TCP if not UDP)
  int index;
  int c, rc;
  
  opterr = 0;

  /*getopt checks for options
   * -S signifies server mode for the program
   * -C signifies client mode for the program, this is the assumed default as well
   * -p Client and Server, specifies which port to use
   * -d Sever Only, specifies database location
   * -h Client Only, specifies host address
   * -m Client Only, initial message te be sent on connection
   * -t Client Only, initial time stamp for initial message
   * -U Client Only, specifies to connect to server in UDP
   * -T Client Only, specifies to connect to server in TCP*/
  while ((c = getopt (argc, argv, "SCD::p:d:h:m:t:UT")) != -1)
    switch (c)
      {
      case 'S':
        is_s = 1;
        break;
      case 'C':
        is_c = 1;
        break;
      case 'D':
	time_out = atoi(optarg);
	break;
	
      case 'p':
        pvalue = optarg;
        break;
      case 'd':
        dvalue = optarg;
        break;
      case 'h':
        hvalue = optarg;
        break;
      case 'm':
        mvalue = optarg;
        break;
      case 't':
        tvalue = optarg;
        break;
      case 'U':
        is_UDP = 1;
        break;
      case 'T':
	//This does nothing, because UDP is already toggled off...
        break;
      case '?':
        if (optopt == 'p')
          fprintf (stderr, "-%c requires an argument.\n", optopt);
	else if (optopt == 'd')
	  fprintf (stderr, "-%c requires an argument.\n", optopt);
	else if (optopt == 'h')
	  fprintf (stderr, "-%c requires an argument.\n", optopt);
	else if (optopt == 'm')
	  fprintf (stderr, "-%c requires an argument.\n", optopt);
	else if (optopt == 't')
	  fprintf (stderr, "-%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }
  for (index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);
  
  //Check for server or client
  if (is_s){
    //create struct for server thread variables
    struct serverThreadParams {
      char *port;
      char *database;
      int *_timeout;
    };
    //allocate values to struct
    struct serverThreadParams *serverParams;
    serverParams = malloc(sizeof(*serverParams));
    serverParams->port=pvalue;
    serverParams->database=dvalue;
    serverParams->_timeout =time_out;
    
    // creat thread
    pthread_t s;
    pthread_create(&s, NULL, do_server,serverParams);
    //do_server(pvalue, dvalue);
    
  }else if (is_c){
    //create struct for client thread variables
    struct clientThreadParams {
      char *_host;
      char *_port;
      char *_message;
      char *_timestamp;
      int _is_udp;
    };
    
    //allocate values for thread
    struct clientThreadParams *clientParams;
    clientParams = malloc(sizeof(*clientParams));
    clientParams->_host = hvalue;
    clientParams->_port = pvalue;
    clientParams->_message = mvalue;
    clientParams->_timestamp = tvalue;
    clientParams->_is_udp = is_UDP;
    
    //do the thread
    pthread_t c;
    pthread_create(&c, NULL, do_client,clientParams);
    
    //do_client(pvalue,hvalue,mvalue,tvalue,is_UDP);
  }else{
    printf("You did not specify client or server. \n");
    exit(1);
  }
  pthread_exit(NULL);
}
