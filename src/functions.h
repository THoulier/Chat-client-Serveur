#include "msg_struct.h"

void send_msg(int client_fd, struct message msgstruct, char * buffer);
int channel_name_validity (char * name, int client_fd);
int nickname_validity (char *  nickname, int client_fd);
int treating_messages(struct message msgstruct, char * buff, int client_fd, int client_nb);