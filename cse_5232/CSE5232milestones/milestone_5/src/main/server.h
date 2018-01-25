//server header

void *do_server(void* args);
void *listen_TCP(void *args);
void *listen_UDP(void *args);
char * messageProcessor(char message[], char dbfilepath[],int msgln);
char * add_peer(char message[], char dbfilepath[]);
char * get_peers(char dbfilepath[]);
char * add_gossip(char message[], char dbfilepath[]);
void broadcastTCP(char message[], char dbfilepath[]);
void broadcastUDP(char message[], char dbfilepath[]);
void send_gossip(int port, char* address, char* gossip_message);
void *tcp_thread_stuff(void *args);
void *udp_thread_stuff(void *args);
void update_peer_tick(char* db, char* address);