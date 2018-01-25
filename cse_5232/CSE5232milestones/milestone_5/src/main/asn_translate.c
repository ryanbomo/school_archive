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
 * This is our ANS1 code.  This works with our ASN1 stuff and creates messages
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <sys/types.h>
#include <netdb.h> 
#include <vector>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <libtasn1.h>

#define BUFF 1024

vector<string> parseCommand (string cmd){
	//Parses the string command 'cmd', retrning a string vector of the individual parts.

	char * mycmd;
	vector<string> list;

	mycmd = strtok ((char *)cmd.c_str(),";");
	while (mycmd != NULL){
		list.push_back(mycmd);
		mycmd = strtok (NULL, ";");
	}
	return list;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////This function created by Sarah Alyousefi Groups to encode the client message berfore sending it////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
int EncodeMsg(char *msg, char *dataBuff){

	asn1_node definitions = NULL, node = NULL;
	char errorDescription[ASN1_MAX_ERROR_DESCRIPTION_SIZE];
	char bufhash[buff], bufmessage[buff], buftime[buff];
	char bufname[buff], bufport[buff], bufip[buff];
	int final_msg = 0;
	int leg = 1024 , Tg = -1;
	
	
	vector<string> list = parseCommand(msg); 


	if(list.at( 0 ).compare("Gossip") == 0){
		Tg = APPLICATION 1;
	}
	else if(list.at( 0 ).compare("peer") == 0){
		Tg = APPLICATION 2;
	}
	
	else if(list.at( 0 ).compare("PeersQuery") == 0){
		Tg = APPLICATION 3;
	}
	
	final_msg = asn1_parser2tree ("GossipProtocol.asn", &definitions, errorDescription);

	switch(Tg) {
		
	case 'APPLICATION 1':  //	Gossip 
	
		final_msg = asn1_create_element(definitions, "GossipProtocol.Gossip", &node );
		strcpy( bufhash ,  list.at( 1 ).c_str() );
		strcpy( buftime ,  list.at( 2 ).c_str() );
		strcpy( bufmessage ,  list.at( 3 ).c_str() );
		// ASN1_write function 
		final_msg = asn1_write_value(node, "sha256hash", bufhash, 1);
		final_msg = asn1_write_value(node, "timestamp", buftime, strlen(buftime));
		final_msg = asn1_write_value(node, "message", bufmessage, strlen(bufmessage));
		//ASN1_der_coding function
		final_msg = asn1_der_coding (node, "GossipProtocol.Gossip", dataBuff, &leg, errorDescription);
		if(final_msg != ASN1_SUCCESS) {
			asn1_perror (final_msg);
			printf("Error with the Encoding function = \"%s\"\n", errorDescription);
			return -1;
		}
		break;
		
		
	case 'APPLICATION 2': //	Peer 
	 
	    final_msg= asn1_create_element(definitions, "GossipProtocol.Peer", &node );
		strcpy( bufname ,  list.at( 1 ).c_str() );
		strcpy( bufport ,  list.at( 2 ).c_str() );
		strcpy( bufip   ,  list.at( 3 ).c_str() );
		//ASN1_write_value function
		final_msg = asn1_write_value(node, "name", bufname, 1);
		final_msg = asn1_write_value(node, "port", bufport, strlen(bufport));
		final_msg = asn1_write_value(node, "ip", bufip, strlen(bufip));
		//ASN1_der_coding function
		final_msg = asn1_der_coding (node, "GossipProtocol.Peer", dataBuff, &leg, errorDescription);
		if(final_msg != ASN1_SUCCESS) {
			asn1_perror (final_msg);
			printf("Error with the Encoding function = \"%s\"\n", errorDescription);
			return -1;
		}
		break;
	case 'APPLICATION 3': 	//	PeersQuery 
		
	    final_msg= asn1_create_element(definitions, "GossipProtocol.PeersQuery", &node );
                          
		//ASN1_write_value function                                                      
		final_msg = asn1_write_value(node, "", NULL, 0); //  <<<-------------------------
		
		//ASN1_der_coding function
		final_msg = asn1_der_coding (node, "GossipProtocol.PeersQuery", dataBuff, &leg, errorDescription);
		if(final_msg != ASN1_SUCCESS) {
			asn1_perror (final_msg);
			printf("Error with the Encoding function = \"%s\"\n", errorDescription);
			return -1;
			}
		break;
		
		}
		
		
		// I put the leave step after the whole process
		//of the gossip protocol because the leave 
		//definition is in different file
		if(list.at( 0 ).compare("Leave") == 0){
		final_msg = asn1_parser2tree ("GossipProtocolLeave.asn", &definitions, errorDescription);
	    // create element 
		final_msg = asn1_create_element(definitions, "GossipProtocolLeave.Leave", &node );
		//ASN1_write_value function
		final_msg = asn1_write_value(node, "name", bufname, 1);
      	//ASN1_der_coding function
		final_msg = asn1_der_coding (node, "GossipProtocolLeave.Leave", dataBuff, &leg, errorDescription);
		if(final_msg != ASN1_SUCCESS) {
			asn1_perror (final_msg);
			printf("Error with the Encoding function = \"%s\"\n", errorDescription);
			return -1;
			}
		}
		
	asn1_delete_structure (&node);
	asn1_delete_structure (&definitions);

	return leg;
}

///////////////// //////////////////////////////////////////////////////////////////////////////////////////////
//////This function created by Sarah Alyousefi Groups to decode the message by the client berfore sending it////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DecodeMsg(char *dataBuff, int Length, int Tg){
	asn1_node definitions = NULL, node = NULL;
	char errorDescription[ASN1_MAX_ERROR_DESCRIPTION_SIZE];
	char bufname[buff], bufport[buff], bufip[buff], char recmsg[buff];
	int final_msg = 0;
	int leg , i;
	char temp_buf[buff];
	Tg = 1;

	final_msg = asn1_parser2tree ("GossipProtocol.asn", &definitions, errorDescription);

	//PeersAnswer
		len = Length;
		final_msg = asn1_create_element(definitions, "GossipProtocol.PeersAnswer", &node );
		final_msg = asn1_der_decoding (&node, dataBuff, leg, errorDescription);

		if(final_msg != ASN1_SUCCESS) {
			asn1_perror (final_msg);
			printf("Error with Decoding function = \"%s\"\n", errorDescription);
			break;
			}
		printf("Message from the server..\n");
		
		for( i=1 ;; i++){

			snprintf(recmsg , sizeof(recmsg), "PeersAnswer.name?%d", i);
			leg= buff;
			final_msg = asn1_read_value(node, recmsg, bufname, &leg);
			if (final_msg==ASN1_ELEMENT_NOT_FOUND){
				printf(" %d Peers Answer received.\n", i-1);
				break;
				}
			printf("\tName [%s] =>\"%s\"\n", recmsg, bufname);

			snprintf(recmsg, sizeof(recmsg), "PeersAnswer.port?%d", i);
			leg = buff;
			final_msg = asn1_read_value(node, recmsg, bufport, &leg);
			printf("\tPort [%s] =>\"%s\"\n", recmsg, bufport);

			snprintf(recmsg, sizeof(recmsg), "PeersAnswer.ip?%d", i);
			leg = buff;
			final_msg = asn1_read_value(node, recmsg, bufip, &leg);
			printf("\tIP [%s] =>\"%s\"\n", recmsg, bufip);

			
			leg = buff;
			final_msg = asn1_read_value(node, recmsg, temp, &leg);
			if (final_msg == ASN1_ELEMENT_NOT_FOUND){
				printf(" %d Peers Answer received.\n", i+1);
				break;
			}

			if (final_msg! = ASN1_SUCCESS) {
				break;
			}
		}
	
	asn1_delete_structure (&node);
	asn1_delete_structure (&definitions);
}
