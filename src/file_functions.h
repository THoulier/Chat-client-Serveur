#include "msg_struct.h"


void send_msg_to_server(int sock_fd, struct message msgstruct, char * buffer);
void file_accepted_preparation(char * buff, char * name, int sock_fd, char * file_sender_nickname);
void file_rejected_preparation(char * buff, char * name, int sock_fd, char * file_sender_nickname);
int peer2peer_connection();