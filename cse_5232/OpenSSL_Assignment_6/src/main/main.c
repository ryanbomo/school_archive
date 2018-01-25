/* ------------------------------------------------------------------------- */
/*   Copyright (C) 2017
                Author:  rbomalaski2015@my.fit.edu
                
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

//Using code from : http://fm4dd.com while working on implementing what I need

//standard stuff includes
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>

//tcp includes
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

//openssl includes
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

#define CERTFILE “client.pem”


int create_socket(char[], BIO *);
int connect_url(char *url_destination, char * url_info, char* cert_path);
struct url_info process_url(char * up_url);



int
main (int argc, char **argv)
{
  char *svalue, *cvalue, *ivalue;
  int c,index;
  
  opterr = 0;

   //getopt checks for options
   // s - stuff before addr [https://www.google.com]/otherstuffafter
   // i - stuff that comes after addr - https://www.google.com[/otherstuffafter]
   // c - path to cert to use
  
  
  while ((c = getopt (argc, argv, "s::c::i::")) != -1)
    switch (c)
      {
      case 's':
        svalue = optarg;
        break;
      case 'i':
	ivalue = optarg;
	break;
      case 'c':
	cvalue = optarg;
	break;
      case '?':
        if (optopt == 's')
          fprintf (stderr, "-%c requires an argument.\n", optopt);
	else if (optopt == 'i')
          fprintf (stderr, "-%c requires an argument.\n", optopt);
	else if (optopt == 'c')
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
  
  printf("s value is: %s\n",svalue);
  printf("i value is: %s\n", ivalue);
  printf("c value is: %s\n",cvalue);
  

  

  connect_url(svalue, ivalue,cvalue);
 
  
}


int connect_url(char *url_destination, char* url_info, char* cert_path) {

  char *dest_url = url_destination;
  BIO *certbio = NULL;
  BIO *outbio = NULL;
  X509 *cert = NULL;
  X509_NAME *certname = NULL;
  const SSL_METHOD *method;
  SSL_CTX *ctx;
  SSL *ssl;
  int server = 0;
  int ret, i;
  EVP_PKEY *pkey = NULL;
  const char cert_filestr[] = "./client.pem";

  OpenSSL_add_all_algorithms();
  ERR_load_BIO_strings();
  ERR_load_crypto_strings();
  SSL_load_error_strings();

  certbio = BIO_new(BIO_s_file());
  outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

  if(SSL_library_init() < 0)
    BIO_printf(outbio, "In connect_url(): Unable to initialize the OpenSSL library !\n");

  method = SSLv23_client_method();


  if ( (ctx = SSL_CTX_new(method)) == NULL)
    BIO_printf(outbio, "In connect_url(): Unable to create a new SSL context structure.\n");


  SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
  
  //add cert file
  
    ret = BIO_read_filename(certbio, cert_path);
  if (! (cert = PEM_read_bio_X509(certbio, NULL, 0, NULL))) {
    BIO_printf(outbio, "Error loading cert into memory\n");
    exit(-1);
  }
    if ((pkey = X509_get_pubkey(cert)) == NULL)
    BIO_printf(outbio, "Error getting public key from certificate");


  // output for cert file based on type    
  if (pkey) {
    switch (EVP_PKEY_id(pkey)) {
      case EVP_PKEY_RSA:
        BIO_printf(outbio, "%d bit RSA Key\n\n", EVP_PKEY_bits(pkey));
        break;
      case EVP_PKEY_DSA:
        BIO_printf(outbio, "%d bit DSA Key\n\n", EVP_PKEY_bits(pkey));
        break;
      default:
        BIO_printf(outbio, "%d bit non-RSA/DSA Key\n\n", EVP_PKEY_bits(pkey));
        break;
    }
  }
   
  ssl = SSL_new(ctx);
  server = create_socket(dest_url, outbio);
  if(server != 0)
    BIO_printf(outbio, "Successfully made the TCP connection to: %s.\n", dest_url);

  SSL_set_fd(ssl, server);

  if ( SSL_connect(ssl) != 1 )
    BIO_printf(outbio, "In connect_url(): Error: Could not build a SSL session to: %s.\n", dest_url);
  else
    BIO_printf(outbio, "Successfully enabled SSL/TLS session to: %s.\n", dest_url);

//get remote cert into x509
  cert = SSL_get_peer_certificate(ssl);
  if (cert == NULL)
    BIO_printf(outbio, "In connect_url(): Error: Could not get a certificate from: %s.\n", dest_url);
  else
    BIO_printf(outbio, "Retrieved the server's certificate from: %s.\n", dest_url);

  certname = X509_NAME_new();
  certname = X509_get_subject_name(cert);

// disply cert data
  BIO_printf(outbio, "Displaying the certificate subject data:\n");
  X509_NAME_print_ex(outbio, certname, 0, 0);
  BIO_printf(outbio, "\n");
  
  
  // GET STUFF HERE
  
  

//get rid of that shiz!
  SSL_free(ssl);
  close(server);
  X509_free(cert);
  SSL_CTX_free(ctx);
  BIO_printf(outbio, "Finished SSL/TLS connection with server: %s.\n", dest_url);
  return(0);
}


int create_socket(char* url_str, BIO *out) {
    int sockfd;
  char hostname[256] = "";
  char    portnum[6] = "443";
  char      protoc[6] = "";
  char      *tmp_ptr = NULL;
  int           port;
  struct hostent *host;
  struct sockaddr_in dest_addr;

  //Take off '/' character if one is trailing
  if(url_str[strlen(url_str)-1] == '/')
    url_str[strlen(url_str)-1] = '\0';

  //get protocol
  strncpy(protoc, url_str, (strchr(url_str, ':')-url_str));


  // get host name
  strncpy(hostname, strstr(url_str, "://")+3, sizeof(hostname));



  port = atoi(portnum);
  printf("Host name is %s\n",hostname);
  if ( (host = gethostbyname(hostname)) == NULL ) {
    BIO_printf(out, "In create_socket(): Error: Cannot resolve hostname %s.\n",  hostname);
    abort();
  }

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  dest_addr.sin_family=AF_INET;
  dest_addr.sin_port=htons(port);
  dest_addr.sin_addr.s_addr = *(long*)(host->h_addr);

  memset(&(dest_addr.sin_zero), '\0', 8);

  tmp_ptr = inet_ntoa(dest_addr.sin_addr);


  if ( connect(sockfd, (struct sockaddr *) &dest_addr,
                              sizeof(struct sockaddr)) == -1 ) {
    BIO_printf(out, "In create_socket(): Error: Cannot connect to host %s [%s] on port %d.\n",
             hostname, tmp_ptr, port);
  }

  return sockfd;
}

// KEEPING FOR FUTURE working
/*
struct url_info{
  char * host_addr;
  char * trailing_info;
};

typedef struct     {
  char host_addr[1024];
  char trailing_info[1024];
} url_info;

struct url_info process_url(char* up_url)
{
  char * holder = up_url;
  struct url_info myinfo;
  int i, s, old_s;
  int third_loc = 0;
  int count = 0;
  
  // I want to find the third istance of the '/' character
  for (i=0, s=0; up_url[i]; i++){
    old_s = s;
    s += (up_url[i] == '/');
    if (old_s!=s){
      count++;
      if (count == 3){
	//printf("It happened. Loc is %d\n", i);
	third_loc = i;
      }
    }
  }
  
  
  if (third_loc == 0){  
    char * trunc_url = holder;
    char * trunc_stuff = "";
    myinfo.host_addr = trunc_url;
    myinfo.trailing_info = trunc_stuff;
  }else if (third_loc >0){
    
    char trunk_url[third_loc];
    char trunk_stuff[strlen(up_url)-third_loc+1];
    
    for (i=0;up_url[i];i++){
      if (i<third_loc){
	// ad to trunk_url
	trunk_url[i] =up_url[i];
      }else{
	// add to trunk_stuff
	trunk_stuff[i-third_loc] = up_url[i];
      }
    }
    
	trunk_stuff[i-third_loc] = '\0';
	trunk_url[third_loc] = '\0';
    
    int d,f;
    printf("Left side is %s\n",trunk_url);
    d = strlen(trunk_url);
    printf("Left side size is %d\n",d);
    printf("Right side is %s\n",trunk_stuff);
    f = strlen(trunk_stuff);
    printf("Right side size is %d\n",f);
   
    //char * thing1 = trunk_url;
    //char * thing2 = trunk_stuff;
    
    myinfo.host_addr = trunk_url;
    myinfo.trailing_info = trunk_stuff;
    printf("myinfo.host_addr is %s\n",myinfo.host_addr);
    printf("myinfo.trailing_info is %s\n",myinfo.trailing_info);
    
    
    // do different thing
  }

  
  return myinfo;
}
*/
