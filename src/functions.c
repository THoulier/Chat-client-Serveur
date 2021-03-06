#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <ctype.h>
#include <time.h>


#include "liste_chainee.h"
#include "common.h"
#include "channel_list.h"
#include "functions.h"

void send_msg(int client_fd, struct message msgstruct, char * buffer){
	write(client_fd,&msgstruct, sizeof(msgstruct));
	write(client_fd, buffer, msgstruct.pld_len);
}

int channel_name_validity (char * name, int client_fd){
	/* verify the nickname validity */
	char msg_tosend[MSG_LEN];
	struct message msgstruct_tosend;
	struct channel * first_channel = malloc(sizeof(*first_channel));

	memset(&msgstruct_tosend,0,sizeof(msgstruct_tosend));
	memset(msg_tosend, 0, MSG_LEN);

	first_channel = channel_list->first;

	if (strlen(name) > NICK_LEN){
		strcpy(msg_tosend,"The channel name must have between 1 and 128 characters");
		strncpy(msgstruct_tosend.nick_sender, "Server", 6);
		strncpy(msgstruct_tosend.infos, "Error", strlen("Error"));            
		msgstruct_tosend.type = MULTICAST_CREATE;
        msgstruct_tosend.pld_len = strlen(msg_tosend);	
		send_msg(client_fd, msgstruct_tosend, msg_tosend);
		return 1;
	}

	for (int i = 0; i<strlen(name); i++){
		if (isalpha(name[i]) == 0){
			strcpy(msg_tosend,"The channel name must not contained special characters nor spaces");
			strncpy(msgstruct_tosend.nick_sender, "Server", 6);
			strncpy(msgstruct_tosend.infos, "Error", strlen("Error"));            
			msgstruct_tosend.type = MULTICAST_CREATE;
			msgstruct_tosend.pld_len = strlen(msg_tosend);	
			send_msg(client_fd, msgstruct_tosend, msg_tosend);
			return 1;
		}
	}

	int exist = 0;

	while (first_channel != NULL){
		if (strcmp(first_channel->name, name) == 0){
			exist = 1;
		}
		first_channel=first_channel->next;
	}
	
	if (exist == 1){
		strcpy(msg_tosend,"The channel name already exists");
		strncpy(msgstruct_tosend.nick_sender, "Server", 6);
		strncpy(msgstruct_tosend.infos, "Error", strlen("Error"));            
		msgstruct_tosend.type = MULTICAST_CREATE;
		msgstruct_tosend.pld_len = strlen(msg_tosend);
		send_msg(client_fd, msgstruct_tosend, msg_tosend);
		return 1;
	}

	free(first_channel);
	return 0;
}


int nickname_validity (char *  nickname, int client_fd){
	/* verify the nickname validity */
	char msg_tosend[MSG_LEN];
	struct message msgstruct_tosend;
	struct client * first_client = malloc(sizeof(*first_client));

	memset(&msgstruct_tosend,0,sizeof(msgstruct_tosend));
	memset(msg_tosend, 0, MSG_LEN);

	first_client = list_client->first;

	if (strlen(nickname) > NICK_LEN){
		strcpy(msg_tosend,"Your nickname must have between 1 and 128 characters");
		strncpy(msgstruct_tosend.nick_sender, "Server", 6);
		strncpy(msgstruct_tosend.infos, "Error", strlen("Error"));            
		msgstruct_tosend.type = NICKNAME_NEW;
        msgstruct_tosend.pld_len = strlen(msg_tosend);	
		send_msg(client_fd, msgstruct_tosend, msg_tosend);
		return 1;
	}

	for (int i = 0; i<strlen(nickname); i++){
		if (isalpha(nickname[i]) == 0){
			strcpy(msg_tosend,"Your nickname must not contained special characters nor spaces");
			strncpy(msgstruct_tosend.nick_sender, "Server", 6);
			strncpy(msgstruct_tosend.infos, "Error", strlen("Error"));            
			msgstruct_tosend.type = NICKNAME_NEW;
			msgstruct_tosend.pld_len = strlen(msg_tosend);	
			send_msg(client_fd, msgstruct_tosend, msg_tosend);
			return 1;
		}
	}

	int exist = 0;

	while (first_client != NULL){
		if (strcmp(first_client->nickname, nickname) == 0){
			exist = 1;
		}
		first_client=first_client->next;
	}
	
	if (exist == 1){
		strcpy(msg_tosend,"Your nickname already exists");
		strncpy(msgstruct_tosend.nick_sender, "Server", 6);
		strncpy(msgstruct_tosend.infos, "Error", strlen("Error"));            
		msgstruct_tosend.type = NICKNAME_NEW;
		msgstruct_tosend.pld_len = strlen(msg_tosend);
		send_msg(client_fd, msgstruct_tosend, msg_tosend);
		return 1;
	}

	free(first_client);
	return 0;
}


int treating_messages(struct message msgstruct, char * buff, int client_fd, int client_nb){
	/* treating the received msg and sending the good response */
	char msg_tosend[MSG_LEN];
	char * nick_list = malloc(sizeof(nick_list));

	struct message msgstruct_tosend;
	struct client * current_client = malloc(sizeof(*current_client));
	struct client * first_client = malloc(sizeof(*first_client));
	struct client * client_nick = malloc(sizeof(*client_nick));
	struct channel * first_channel = malloc(sizeof(*first_channel));
	struct channel * channel_name = malloc(sizeof(*channel_name));

	memset(&msgstruct_tosend,0,sizeof(msgstruct_tosend));
	memset(msg_tosend, 0, MSG_LEN);

	current_client = find_client(client_fd,list_client);
	first_client = list_client->first;
	client_nick = find_client_nickname(msgstruct.infos, list_client);
	first_channel = channel_list->first;
	

	switch (msgstruct.type){
		
		case NICKNAME_NEW:
			if (nickname_validity(msgstruct.infos, client_fd) == 1){
				return 1;
			}
			else {
				if (!strcmp(msgstruct.nick_sender,"")){
					sprintf(msg_tosend,"Welcome on the chat %s" ,buff);
				}
				else {
					/* update nickname */
					sprintf(msg_tosend,"Your new nickname is %s" ,buff);
				}

				update_nickname(current_client, msgstruct.infos); //add in the list
				strncpy(msgstruct_tosend.nick_sender, "Server", 6);
				strncpy(msgstruct_tosend.infos, msgstruct.infos, strlen(msgstruct.infos));            
				msgstruct_tosend.type = NICKNAME_NEW;
				msgstruct_tosend.pld_len = strlen(msg_tosend);
			}
		break;

		case ECHO_SEND:
			if (!strcmp(buff,"/quit")){
				return 0; //connection closed by client
			}
			sprintf(msg_tosend,"[%s] : %s", msgstruct.nick_sender, buff);
			strncpy(msgstruct_tosend.infos, "\0", 1);
            strncpy(msgstruct_tosend.nick_sender, msgstruct.nick_sender, strlen(msgstruct.nick_sender));
            msgstruct_tosend.type = ECHO_SEND;
            msgstruct_tosend.pld_len = strlen(msg_tosend);

        break;

		case NICKNAME_LIST:
			strcat(msg_tosend, "Online users are : \n");
			while (first_client != NULL){
				sprintf(nick_list, "		- %s\n",first_client->nickname);
				strcat(msg_tosend, nick_list);
				first_client=first_client->next;
			}
			strcpy(msgstruct_tosend.infos, "Users connected");
            strncpy(msgstruct_tosend.nick_sender, "Server", 6);
            msgstruct_tosend.type = NICKNAME_LIST;
            msgstruct_tosend.pld_len = strlen(msg_tosend);
        break;

		case NICKNAME_INFOS:
			if (client_nick == NULL){
				sprintf(msg_tosend, "User %s does not exist", buff);
				sprintf(msgstruct_tosend.nick_sender, "Server");
				strcpy(msgstruct_tosend.infos, "Error");
				msgstruct_tosend.type = NICKNAME_INFOS;
				msgstruct_tosend.pld_len = strlen(msg_tosend);
			}
			else{
				sprintf(msg_tosend, "%s connected since %s, with IP adress %s and port number %d\n",client_nick->nickname, client_nick->connection_time, client_nick->adress, client_nick->port);
				sprintf(msgstruct_tosend.infos, "%s", client_nick->nickname);
				sprintf(msgstruct_tosend.nick_sender, "Server");
				msgstruct_tosend.type = NICKNAME_INFOS;
				msgstruct_tosend.pld_len = strlen(msg_tosend);
			}
        break;

		case BROADCAST_SEND:
			msgstruct_tosend.type = BROADCAST_SEND;
			sprintf(msg_tosend,"[%s] : %s", msgstruct.nick_sender, buff);
			strncpy(msgstruct_tosend.infos, "\0", 1);
			strcpy(msgstruct_tosend.nick_sender, msgstruct.nick_sender);
			msgstruct_tosend.pld_len = strlen(msg_tosend);
			while (first_client != NULL){
				if (strcmp(first_client->nickname, msgstruct.nick_sender) != 0){
					send_msg(first_client->fd,msgstruct_tosend,msg_tosend);
				}
				first_client=first_client->next;
			}
			printf("[Client %i] : %s\n", client_nb,buff);
			//printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);
			return 1;
		break;

		case UNICAST_SEND:

			
			if (client_nick == NULL){
				msgstruct_tosend.type = UNICAST_SEND;
				sprintf(msg_tosend, "User %s does not exist", msgstruct.infos);
				strncpy(msgstruct_tosend.infos, "Error", strlen("Error"));
				strcpy(msgstruct_tosend.nick_sender, msgstruct.nick_sender);
				msgstruct_tosend.pld_len = strlen(msg_tosend);
			}
			else{
				msgstruct_tosend.type = UNICAST_SEND;
				sprintf(msg_tosend,"[%s] : %s", msgstruct.nick_sender, buff);
				strncpy(msgstruct_tosend.infos, "\0", 1);
				strcpy(msgstruct_tosend.nick_sender, msgstruct.nick_sender);
				msgstruct_tosend.pld_len = strlen(msg_tosend);
				while (first_client != NULL){
					if (strcmp(first_client->nickname, msgstruct.infos) == 0){
						send_msg(first_client->fd,msgstruct_tosend,msg_tosend);
					}
					first_client=first_client->next;
				}
				return 1;
			}
			printf("[Client %i] : %s\n", client_nb,buff);
			//printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);
			
		break;

		case MULTICAST_CREATE:
			if (channel_name_validity(msgstruct.infos, client_fd) == 1){
				return 1;
			}
			else {
				channel_insertion(channel_list, client_fd, msgstruct.infos);
				sprintf(msg_tosend,"[Server]: You have created channel %s \n%s > You have joined channel %s", msgstruct.infos, msgstruct.infos, msgstruct.infos);
				strcpy(msgstruct_tosend.nick_sender, msgstruct.nick_sender);
				sprintf(msgstruct_tosend.infos, "%s", msgstruct.infos);
				msgstruct_tosend.type = MULTICAST_CREATE;
				msgstruct_tosend.pld_len = strlen(msg_tosend);
			}
        break;

		case MULTICAST_LIST:
			strcat(msg_tosend, "Online channels are : \n");
			while (first_channel != NULL){
				sprintf(nick_list, "		- %s\n",first_channel->name);
				strcat(msg_tosend, nick_list);
				first_channel=first_channel->next;
			}
			strcpy(msgstruct_tosend.infos, "\0");
            strncpy(msgstruct_tosend.nick_sender, "Server", 6);
            msgstruct_tosend.type = MULTICAST_LIST;
            msgstruct_tosend.pld_len = strlen(msg_tosend);
        break;

		case MULTICAST_JOIN:
			channel_name = find_channel_name(msgstruct.infos, channel_list); //the client wants to join this channel
			if (channel_name == NULL){
				/* if the channel does not exist */
				msgstruct_tosend.type = MULTICAST_JOIN;
				sprintf(msg_tosend, "[Server] : Channel %s does not exist", msgstruct.infos);
				strncpy(msgstruct_tosend.infos, "Error", strlen("Error"));
				strncpy(msgstruct_tosend.nick_sender, "Server", 6);
				msgstruct_tosend.pld_len = strlen(msg_tosend);
				send_msg(client_fd, msgstruct_tosend, msg_tosend);
			}
			else{
				while (first_channel != NULL){
					/* if the client was in a channel, he quits this channel */
					for (int i=0; i<MAXCLI; i++){
						if (first_channel->fds[i] == client_fd){
							first_channel->fds[i] = -1;
						}
					}
					if (channel_is_empty(first_channel)){
						/* if the channel quited is empty, it is supressed */
						strncpy(msgstruct_tosend.infos, "", 0);
						strncpy(msgstruct_tosend.nick_sender, "Server", 6);
						sprintf(msg_tosend, "[Server] : Channel %s has been destroyed", first_channel->name);
						msgstruct_tosend.pld_len = strlen(msg_tosend);
						msgstruct_tosend.type = MULTICAST_JOIN;
						channel_suppression(first_channel, channel_list);
						send_msg(client_fd,msgstruct_tosend,msg_tosend);
					}
					first_channel = first_channel->next;
				}
				int cpt = 0;
				while (channel_name->fds[cpt] != -1){
					cpt ++;
				}
				/* add the client to the new channel */
				channel_name->fds[cpt] = client_fd;
				strncpy(msgstruct_tosend.nick_sender, "Server", 6);
				sprintf(msgstruct_tosend.infos, "%s", msgstruct.infos);
				msgstruct_tosend.type = MULTICAST_JOIN;
				for (int i=0; i<MAXCLI; i++){
					if (channel_name->fds[i] != -1){
						if (channel_name->fds[i] == client_fd){
							sprintf(msg_tosend,"[Server]: You have joined channel %s", msgstruct.infos);
							msgstruct_tosend.pld_len = strlen(msg_tosend);
							send_msg(channel_name->fds[i],msgstruct_tosend,msg_tosend);
						}
						else {
							sprintf(msg_tosend,"%s > %s has joined the channel",msgstruct.infos, msgstruct.nick_sender);
							msgstruct_tosend.pld_len = strlen(msg_tosend);
							send_msg(channel_name->fds[i],msgstruct_tosend,msg_tosend);
						}
					}
				}
				//printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);
				return 1;
			}
        break;

		case MULTICAST_QUIT:
			channel_name = find_channel_name(msgstruct.infos, channel_list);
			if (channel_name == NULL){
				msgstruct_tosend.type = MULTICAST_QUIT;
				sprintf(msg_tosend, "[Server] : Channel %s does not exist", msgstruct.infos);
				strncpy(msgstruct_tosend.infos, "Error", strlen("Error"));
				strncpy(msgstruct_tosend.nick_sender, "Server", 6);
				msgstruct_tosend.pld_len = strlen(msg_tosend);
			}
			else{
				for (int i=0; i<MAXCLI; i++){
					/* supress the client from the channel fds */
					if (channel_name->fds[i] != -1){
						if (channel_name->fds[i] == client_fd){
							msgstruct_tosend.type = MULTICAST_QUIT;
							strcpy(msgstruct_tosend.infos, "");
							strncpy(msgstruct_tosend.nick_sender, "Server", 6);
							sprintf(msg_tosend, "[Server] : You have left channel %s", msgstruct.infos);
							msgstruct_tosend.pld_len = strlen(msg_tosend);
							send_msg(channel_name->fds[i],msgstruct_tosend,msg_tosend);
							channel_name->fds[i] = -1;
						}
						else {
							msgstruct_tosend.type = MULTICAST_QUIT;
							strcpy(msgstruct_tosend.infos, msgstruct.infos);
							strncpy(msgstruct_tosend.nick_sender, "Server", 6);
							sprintf(msg_tosend,"%s > %s has left the channel",msgstruct.infos, msgstruct.nick_sender);
							msgstruct_tosend.pld_len = strlen(msg_tosend);
							send_msg(channel_name->fds[i],msgstruct_tosend,msg_tosend);
						}
					}
				}
				if (channel_is_empty(channel_name)){
					/* if the channel quited is empty, it is supressed */
					channel_suppression(channel_name, channel_list);
					strncpy(msgstruct_tosend.infos, "", 0);
					strncpy(msgstruct_tosend.nick_sender, "Server", 6);
					sprintf(msg_tosend, "[Server] : Channel %s has been destroyed", msgstruct.infos);
					msgstruct_tosend.pld_len = strlen(msg_tosend);
					send_msg(client_fd,msgstruct_tosend,msg_tosend);
				}
				//printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);
				return 1;
			}
		break;

		case MULTICAST_SEND:
		
			if (!strcmp(buff,"/quit")){
				/* empeche le client de quitter le chat quand il est dans une channel */
				strcpy(msg_tosend, "[Server] : You can't quit the chat when you're in a channel\n");
				msgstruct_tosend.type = MULTICAST_SEND;
				strcpy(msgstruct_tosend.infos, msgstruct.infos);
				strcpy(msgstruct_tosend.nick_sender, msgstruct.nick_sender);
				msgstruct_tosend.pld_len = strlen(msg_tosend);
				send_msg(client_fd, msgstruct_tosend, msg_tosend);
				return 1;
			}
			channel_name = find_channel_name(msgstruct.infos, channel_list);
			msgstruct_tosend.type = MULTICAST_SEND;
			strcpy(msgstruct_tosend.infos, msgstruct.infos);
			strcpy(msgstruct_tosend.nick_sender, msgstruct.nick_sender);
			for (int i=0; i<MAXCLI; i++){
				/*send the message to all the clients in the channel */
				if (channel_name->fds[i] != -1){
					if (channel_name->fds[i] == client_fd){
						sprintf(msg_tosend,"%s > [Me] : %s",msgstruct.infos, buff);
						msgstruct_tosend.pld_len = strlen(msg_tosend);
						send_msg(channel_name->fds[i],msgstruct_tosend,msg_tosend);
					}
					else {
						sprintf(msg_tosend,"%s > [%s] : %s",msgstruct.infos, msgstruct.nick_sender, buff);
						msgstruct_tosend.pld_len = strlen(msg_tosend);
						send_msg(channel_name->fds[i],msgstruct_tosend,msg_tosend);
					}
				}
			}
			printf("[Client %i] : %s\n", client_nb,buff);
			//printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);
			return 1;
		break;

		case FILE_REQUEST:
			if (client_nick == NULL){
				msgstruct_tosend.type = FILE_REQUEST;
				sprintf(msg_tosend, "User %s does not exist", msgstruct.infos);
				strncpy(msgstruct_tosend.infos, "Error", strlen("Error"));
				strcpy(msgstruct_tosend.nick_sender, msgstruct.nick_sender);
				msgstruct_tosend.pld_len = strlen(msg_tosend);
			}
			else {
				msgstruct_tosend.type = FILE_REQUEST;
				strcpy(msgstruct_tosend.infos,msgstruct.infos);
				sprintf(msg_tosend,"[Server] : %s wants you to accept the transfer of the file named \"%s\". Do you accept? [Y/N]", msgstruct.nick_sender, buff);
				msgstruct_tosend.pld_len = strlen(msg_tosend);
				strcpy(msgstruct_tosend.nick_sender,msgstruct.nick_sender);
				send_msg(client_nick->fd, msgstruct_tosend,msg_tosend);
				return 1;
			}
		break;

		case FILE_ACCEPT:
			if (client_nick == NULL){
				msgstruct_tosend.type = FILE_ACCEPT;
				sprintf(msg_tosend, "User %s does not exist", msgstruct.infos);
				strncpy(msgstruct_tosend.infos, "Error", strlen("Error"));
				strcpy(msgstruct_tosend.nick_sender, msgstruct.nick_sender);
				msgstruct_tosend.pld_len = strlen(msg_tosend);
			}
			else {
				msgstruct_tosend.type = FILE_ACCEPT;
				strcpy(msgstruct_tosend.infos,buff);
				sprintf(msg_tosend,"%s accepted file transfert", msgstruct.nick_sender);
				msgstruct_tosend.pld_len = strlen(msg_tosend);
				strcpy(msgstruct_tosend.nick_sender,msgstruct.nick_sender);
				send_msg(client_nick->fd, msgstruct_tosend,msg_tosend);
				
				printf("[Client %i] : %s\n", client_nb,buff);
				//printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);

				return 1;
			}
		break;

		case FILE_REJECT:
			if (client_nick == NULL){
				msgstruct_tosend.type = FILE_REJECT;
				sprintf(msg_tosend, "User %s does not exist", msgstruct.infos);
				strncpy(msgstruct_tosend.infos, "Error", strlen("Error"));
				strcpy(msgstruct_tosend.nick_sender, msgstruct.nick_sender);
				msgstruct_tosend.pld_len = strlen(msg_tosend);
			}
			else {
				msgstruct_tosend.type = FILE_REJECT;
				strcpy(msgstruct_tosend.infos,msgstruct.infos);
				sprintf(msg_tosend,"%s didn't accept file transfert", msgstruct.nick_sender);
				msgstruct_tosend.pld_len = strlen(msg_tosend);
				strcpy(msgstruct_tosend.nick_sender,msgstruct.nick_sender);
				send_msg(client_nick->fd, msgstruct_tosend,msg_tosend);

				memset(&msgstruct_tosend,0,sizeof(msgstruct_tosend));
				memset(msg_tosend, 0, MSG_LEN);

				msgstruct_tosend.type = FILE_REJECT;
				strcpy(msgstruct_tosend.infos,msgstruct.infos);
				sprintf(msg_tosend,"[Server] : You cancelled file transfert from %s", msgstruct.nick_sender);
				msgstruct_tosend.pld_len = strlen(msg_tosend);
				strcpy(msgstruct_tosend.nick_sender,"Server");
				send_msg(current_client->fd, msgstruct_tosend,msg_tosend);
				return 1;
			}
		break;

		case FILE_ACK:
			msgstruct_tosend.type = FILE_ACK;
			strcpy(msgstruct_tosend.infos,msgstruct.infos);
			strcpy(msg_tosend,buff);
			msgstruct_tosend.pld_len = strlen(msg_tosend);
			strcpy(msgstruct_tosend.nick_sender,msgstruct.nick_sender);
			send_msg(client_nick->fd, msgstruct_tosend,msg_tosend);

			return 1;
		break;
		default:
        break;
	}
	printf("[Client %i] : %s\n", client_nb,buff);
	//printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);
					
	send_msg(client_fd,msgstruct_tosend,msg_tosend);

	return 1;

}