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
#include "msg_struct.h"



void send_msg(int client_fd, struct message msgstruct, char * buffer){
	write(client_fd,&msgstruct, sizeof(msgstruct));
	write(client_fd, buffer, msgstruct.pld_len);
}


void connection_client(int client_nb, int nfds, int server_sock, struct pollfd * fds) {
	int fd = 1;
    struct sockaddr_in client_addr;
    socklen_t size_addr = sizeof(struct sockaddr_in);
    int client_fd = accept(server_sock,(struct sockaddr*)&client_addr,&size_addr);
	char * client_ip = inet_ntoa(client_addr.sin_addr);
	//printf("%d\n",list_client->first->fd);
	
	insertion(list_client,client_fd,client_addr.sin_port,client_ip);

    while(fds[fd].fd != 0 && fd < nfds){
        fd++;    
	}
	if(fd < nfds){

        fds[fd].fd = client_fd;
        fds[fd].events = POLLIN;
        printf("[Client %i] connected\n", fd);
		
    }
	else if (fd > nfds){
        perror("The max number of connections is reached\n");
        close(client_fd);
    }
    fds[client_nb].revents = 0;
}

void disconnection_client(int client_nb, int client_fd, struct pollfd * fds) {
	suppression(find_client(client_fd,list_client),list_client);
	close(client_fd);
	printf("[Client %i] disconnected\n",client_nb);
	memset((fds + client_nb),0,sizeof(struct pollfd));
}

int nickname_validity (char *  nickname, int client_fd){
	char msg_tosend[MSG_LEN];
	struct message msgstruct_tosend;
	struct client * first_client = malloc(sizeof(*first_client));

	memset(&msgstruct_tosend,0,sizeof(msgstruct_tosend));
	memset(msg_tosend, 0, MSG_LEN);

	first_client = list_client->first;

	if (strlen(nickname) >= NICK_LEN){
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
			printf("%d\n",strcmp(first_client->nickname, nickname));
			exist = 1;
		}
		first_client=first_client->next;
	}
	
	printf("%d\n", exist);
	if (exist == 1){
		strcpy(msg_tosend,"Your nickname already exists");
		strncpy(msgstruct_tosend.nick_sender, "Server", 6);
		strncpy(msgstruct_tosend.infos, "Error", strlen("Error"));            
		msgstruct_tosend.type = NICKNAME_NEW;
		msgstruct_tosend.pld_len = strlen(msg_tosend);
		send_msg(client_fd, msgstruct_tosend, msg_tosend);
		return 1;
	}
	return 0;
}


int treating_messages(struct message msgstruct, char * buff, int client_fd, int client_nb){
	char msg_tosend[MSG_LEN];
	char * nick_list = malloc(sizeof(nick_list));

	struct message msgstruct_tosend;
	struct client * current_client = malloc(sizeof(*current_client));
	struct client * first_client = malloc(sizeof(*first_client));
	struct client * client_nick = malloc(sizeof(*client_nick));

	memset(&msgstruct_tosend,0,sizeof(msgstruct_tosend));
	memset(msg_tosend, 0, MSG_LEN);

	current_client = find_client(client_fd,list_client);
	first_client = list_client->first;
	client_nick = find_client_nickname(msgstruct.infos, list_client);

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
					sprintf(msg_tosend,"Your new nickname is %s" ,buff);
				}

				update_nickname(current_client, msgstruct.infos);
				printf("le nom du client est : %s\n", current_client->nickname);
				strncpy(msgstruct_tosend.nick_sender, "Server", 6);
				strncpy(msgstruct_tosend.infos, msgstruct.infos, strlen(msgstruct.infos));            
				msgstruct_tosend.type = NICKNAME_NEW;
				msgstruct_tosend.pld_len = strlen(msg_tosend);
			}
		break;

		case ECHO_SEND:
			if (!strcmp(buff,"/quit")){
				return 0;
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
			/*
			if (strcmp("", buff) ==  0){
				strcpy(msg_tosend,"Please respect /whois <nickname>");
				sprintf(msgstruct_tosend.nick_sender, "Server");
            	msgstruct_tosend.type = NICKNAME_INFOS;
            	msgstruct_tosend.pld_len = strlen(msg_tosend);
				strcpy(msgstruct_tosend.infos, "Error");
				send_msg(client_fd,msgstruct_tosend,msg_tosend);
				return 1;
			}
			*/
			if (client_nick == NULL){
				sprintf(msg_tosend, "User %s does not exist", buff);
				sprintf(msgstruct_tosend.nick_sender, "Server");
				strcpy(msgstruct_tosend.infos, "Error");
				msgstruct_tosend.type = NICKNAME_INFOS;
				msgstruct_tosend.pld_len = strlen(msg_tosend);
			}
			else{
				sprintf(msg_tosend, "%s connected since %s, with IP adress %s and port number %d\n",client_nick->nickname, client_nick->connection_time, client_nick->adress, client_nick->port);
				sprintf(msgstruct_tosend.infos, "informations about %s", client_nick->nickname);
				sprintf(msgstruct_tosend.nick_sender, "Server");
				msgstruct_tosend.type = NICKNAME_INFOS;
				msgstruct_tosend.pld_len = strlen(msg_tosend);
			}

			/*
			while (first_client != NULL){
				if (strcmp(first_client->nickname, buff) == 0){
					sprintf(msg_tosend, "%s connected since %s, with IP adress %s and port number %d\n",first_client->nickname, first_client->connection_time, first_client->adress, first_client->port);
					sprintf(msgstruct_tosend.infos, "informations about %s", first_client->nickname);
				}
				first_client=first_client->next;
			}
			
            sprintf(msgstruct_tosend.nick_sender, "Server");
            msgstruct_tosend.type = NICKNAME_INFOS;
            msgstruct_tosend.pld_len = strlen(msg_tosend);
			*/
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
			printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);
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
			printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);
			
		break;

		default:
        break;
	}
	printf("[Client %i] : %s\n", client_nb,buff);
	printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);
					
	send_msg(client_fd,msgstruct_tosend,msg_tosend);
	return 1;

}




void echo_server(int server_sock) {

	/*Init of poll structure */
	int nfds = MAXCLI;
	struct pollfd fds[nfds];
	memset(fds,0,nfds*sizeof(struct pollfd));

	fds[0].fd = server_sock;
	fds[0].events = POLLIN;

	struct message msgstruct;
	char buffer[MSG_LEN];
	while (1) {
		
        memset(buffer,0,MSG_LEN);
		memset(&msgstruct, 0, sizeof(struct message));
		/* tests on poll function */
		int enabled = 0;
        enabled = poll(fds,nfds,-1);

        if (enabled >0){
            printf("A client wants to communicate\n");
        }
        else{
            printf("There is a problem with poll function: error %i\n",enabled);
            continue;
        }
        
        for (int i = 0; i < nfds; i++){
			/* connection treated with poll function */
            if ( fds[i].revents == POLLIN && i == 0){
				connection_client(i, nfds, server_sock, fds);
            }
            
            else if ((fds[i].revents & POLLHUP )&& i != 0){
                close(fds[i].fd);
                printf("[Client %i] disconnected with pollhup\n",i);
                memset((fds + i),0,sizeof(struct pollfd));
            }

            if (fds[i].revents == POLLIN && i != 0){
			/* interraction with clients */

				int ret_struct = read(fds[i].fd,&msgstruct, sizeof(msgstruct));
                int ret = read(fds[i].fd,buffer,MSG_LEN);
                
                if (-1 == ret && -1 == ret_struct) {
                    perror("Error while reading");
                    break;
                }
				else if (0 == ret && 0 == ret_struct){
					disconnection_client(i, fds[i].fd, fds);
				}
				else if(ret != -1 && -1 != ret_struct){

					if (treating_messages(msgstruct,buffer,fds[i].fd, i) == 0){
						disconnection_client(i, fds[i].fd, fds);
					}
					
    			}
            }    
        }
		display_list(list_client);
	}
}

int handle_bind(int portnb) {
	/* Create socket */
	printf("Creating socket...\n");
	int server_sock = socket(AF_INET, SOCK_STREAM, 0);

	// create server addr
	char  * addr_ip = "127.0.0.1";
	short port = portnb;
	struct sockaddr_in  server_addr;
	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family= AF_INET;
	server_addr.sin_port = htons(port);
	inet_aton(addr_ip,&(server_addr.sin_addr));
		
	// bind to server addr
	printf("Binding...\n");
	if( bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
		perror("Error while binding");
		return 0;
	}
	
	// listen
	printf("Listening...\n");
	if (listen(server_sock, 10) == -1){
		perror("Error while listening");
		return 0;
	}
	return server_sock;
}

int main(int argc, char * argv[]) {
	list_client = initialisation();

	if( argc != 2 ){
    	printf("Missing port number\n");
    	return 0;
  	}
	int sfd;
	sfd = handle_bind(atoi(argv[1]));

	echo_server(sfd);
	close(sfd);

	return EXIT_SUCCESS;
}

