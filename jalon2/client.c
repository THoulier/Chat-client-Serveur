#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include "common.h"
#include "msg_struct.h"
#include "liste_chainee.h"

void send_msg_to_server(int sock_fd, struct message msgstruct, char * buffer){
	if (send(sock_fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
		printf("Error while sending a message structure");
	}

	if (send(sock_fd, buffer, msgstruct.pld_len, 0) <= 0) {
		printf("Error while sending a message");
	}
}

void message_preparation(char * buff, char * name, int sock_fd){
	struct message msgstruct;
	char msg_tosend[MSG_LEN];

	memset(msg_tosend, 0, MSG_LEN);
    memset(&msgstruct, 0, sizeof(struct message));
	printf("%s\n",name);
    if(strncmp(buff, "/nick ", strlen("/nick ")) == 0) {
        msgstruct.type = NICKNAME_NEW;
		strcpy(msg_tosend, strchr(buff, ' ') + 1);
		msgstruct.pld_len = strlen(msg_tosend);
		strncpy(msgstruct.nick_sender, name,strlen(name));
		strncpy(msgstruct.infos, "\0", 1);
		strncpy(name, msg_tosend,strlen(msg_tosend));
	}
	else {
		printf("%s\n",name);
		msgstruct.pld_len = strlen(buff);
		strncpy(msgstruct.nick_sender, name, strlen(name));
		msgstruct.type = ECHO_SEND;
		strncpy(msgstruct.infos, "\0", 1);
		strncpy(msg_tosend, buff, strlen(buff));
	}
	send_msg_to_server(sock_fd,msgstruct,msg_tosend);
	
}

void echo_client(int sockfd) {

	int ret = -1;
	struct pollfd fds[2];
	memset(fds, 0, 2*sizeof(struct pollfd));
	fds[0].events = POLLIN;
	fds[0].fd = sockfd;
	fds[1].events = POLLIN;
	fds[1].fd = STDIN_FILENO;

	char name[NICK_LEN];
	char buff[MSG_LEN];
	struct message msgstruct;
	memset(name, 0, NICK_LEN);
	while (1) {
		
		memset(buff, 0, MSG_LEN);
		memset(&msgstruct, 0, sizeof(struct message));
		ret = poll(fds,2,-1);
        if (ret <=0){
            printf("There is a problem with poll function: error %i\n",ret);
            continue;
        }

		if (fds[0].revents & POLLIN){

			// Receiving structure
			if (recv(sockfd, &msgstruct, sizeof(struct message), 0) <= 0) {
				printf("Error while receiving a structure message\n");
				break;
			}
			// Receiving message
			if (recv(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
				printf("Error while receiving a message\n");
				break;
			}
			printf("[Server]: %s\n", buff);
			printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);

		}

		if (fds[1].revents & POLLIN){


			read(fds[1].fd,buff, MSG_LEN);
			buff[strlen(buff) - 1] = 0;
			message_preparation(buff, name, sockfd);

			printf("Message sent!\n");

            if(strcmp(buff, "/quit") == 0) {
                break;
            }

		}
	}
}

int handle_connect(char address_ip[], int portnb) {

	/* Create socket */
	printf("Creating socket...\n");
    int server_sock =  socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	// create server addr
	char  * addr_ip = address_ip;
	short port = portnb;
	struct sockaddr_in  server_addr;
	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family= AF_INET;
	server_addr.sin_port = htons(port);
	inet_aton(addr_ip,&(server_addr.sin_addr));

    /* connection au serveur */
	printf("Connecting to server ...");
    if (connect(server_sock, (struct sockaddr *)&server_addr,sizeof(server_addr)) == -1 ){
        perror("Error connect");
        exit(1);
    }
	printf("done!\n");
	return server_sock;

}

int main(int argc, char * argv[]) {
	if (argc != 3){
        perror("Port number and IP adress are missing\n");
        return 0;
    }
	int sfd;
	sfd = handle_connect(argv[1],atoi(argv[2]));
	printf("[Server] : Please login with /nick <username>\n");
	echo_client(sfd);
	close(sfd);
	return EXIT_SUCCESS;
}

