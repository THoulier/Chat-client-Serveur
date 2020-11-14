#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include "file_functions.h"
#include "common.h"

void send_msg_to_server(int sock_fd, struct message msgstruct, char * buffer){
	if (send(sock_fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
		printf("Error while sending a message structure");
	}

	if (send(sock_fd, buffer, msgstruct.pld_len, 0) <= 0) {
		printf("Error while sending a message");
	}
}

void file_accepted_preparation(char * buff, char * name, int sock_fd, char * file_sender_nickname){
	struct message msgstruct;
	char msg_tosend[MSG_LEN];
	memset(msg_tosend, 0, MSG_LEN);
    memset(&msgstruct, 0, sizeof(struct message));

	msgstruct.type = FILE_ACCEPT;
	strncpy(msgstruct.nick_sender, name, strlen(name));
	strncpy(msgstruct.infos, file_sender_nickname, strlen(file_sender_nickname));
	strcpy(msg_tosend, "127.0.0.1:8082");
	msgstruct.pld_len = strlen(msg_tosend);
	send_msg_to_server(sock_fd, msgstruct, msg_tosend);

	printf("%d\n",peer2peer_connection());
	
}

void file_rejected_preparation(char * buff, char * name, int sock_fd, char * file_sender_nickname){
	struct message msgstruct;
	char msg_tosend[MSG_LEN];
	memset(msg_tosend, 0, MSG_LEN);
    memset(&msgstruct, 0, sizeof(struct message));

	msgstruct.type = FILE_REJECT;
	strncpy(msgstruct.nick_sender, name, strlen(name));
	strncpy(msgstruct.infos, file_sender_nickname, strlen(file_sender_nickname));
	strncpy(msg_tosend, "\0",1);
	msgstruct.pld_len = strlen(msg_tosend);
	send_msg_to_server(sock_fd, msgstruct, msg_tosend);

}

int peer2peer_connection(){
	/* Create socket */
	printf("Creating socket for peer2peer connection...\n");
	int client_sock = socket(AF_INET, SOCK_STREAM, 0);

	// create server addr
	char  * addr_ip = "127.0.0.1";
	short port = 0;
	struct sockaddr_in  client_addr;
	memset(&client_addr, '\0', sizeof(client_addr));
	client_addr.sin_family= AF_INET;
	client_addr.sin_port = htons(8082);
	inet_aton(addr_ip,&(client_addr.sin_addr));
		
	// bind to server addr
	printf("Binding...\n");
	if( bind(client_sock, (struct sockaddr *)&client_addr, sizeof(client_addr)) == -1){
		perror("Error while binding");
		return 0;
	}
	
	// listen
	printf("Listening...\n");
	if (listen(client_sock, 10) == -1){
		perror("Error while listening");
		return 0;
	}
	return client_sock;
}