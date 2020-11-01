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


void echo_client(int sockfd) {

	int ret = -1;
	struct pollfd fds[2];
	memset(fds, 0, 2*sizeof(struct pollfd));
	fds[0].events = POLLIN;
	fds[0].fd = sockfd;
	fds[1].events = POLLIN;
	fds[1].fd = STDIN_FILENO;

	

	while (1) {
		ret = poll(fds,2,-1);
        if (ret <=0){
            printf("There is a problem with poll function: error %i\n",ret);
            continue;
        }

		if (fds[0].revents & POLLIN){
			char buff[MSG_LEN];
			struct message msgstruct;
			memset(buff, 0, MSG_LEN);
			memset(&msgstruct, 0, sizeof(struct message));
			// Receiving structure
			if (recv(sockfd, &msgstruct, sizeof(struct message), 0) <= 0) {
				break;
			}
			// Receiving message
			if (recv(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
				break;
			}
			printf("[Server]: %s\n", buff);
			printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);

			memset(&msgstruct, 0, sizeof(struct message));
			memset(buff, 0, MSG_LEN);
		}

		if (fds[1].revents & POLLIN){

			char buff[MSG_LEN];
			memset(buff, 0, MSG_LEN);
			struct message msgstruct;
			memset(&msgstruct, 0, sizeof(struct message));

			read(fds[1].fd,buff, MSG_LEN);

			msgstruct.pld_len = strlen(buff);
			strncpy(msgstruct.nick_sender, "Toto", 4);
			msgstruct.type = ECHO_SEND;
			strncpy(msgstruct.infos, "\0", 1);

			if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
				printf("Error while sending a message structure");
				break;
			}
			if (send(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
				printf("Error while sending a message");
				break;
			}
			printf("Message sent!\n");

			memset(&msgstruct, 0, sizeof(struct message));
			memset(buff, 0, MSG_LEN);

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
	echo_client(sfd);
	close(sfd);
	return EXIT_SUCCESS;
}

