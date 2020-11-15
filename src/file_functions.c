#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>

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

int send_file(char * name, int sock_fd, char * file_name){
    struct message msgstruct_tosend;
    char msg_tosend[MSG_LEN];
    memset(&msgstruct_tosend, 0, sizeof(struct message));
    memset(msg_tosend, 0, MSG_LEN);

    printf("Sending the file...\n");

    /* Open the file which will be transfered */
    int fd_in;
    if(-1 == (fd_in = open(file_name, O_RDWR))){
        perror("Error on opening file");
        return 0;
    }
    /* Send the datas of the files by package of MSG_LEN = 1024 bytes maximum size */
    int ret = -1;
    while(ret != 0){

        msgstruct_tosend.type = FILE_SEND;
        strcpy(msgstruct_tosend.nick_sender, name);
        strcpy(msgstruct_tosend.infos, file_name);
        if(-1 == (ret = read(fd_in, msg_tosend, MSG_LEN))){
            perror("Error on reading file");
            return 0;
        }

        if(ret == 0)
        break;

        msgstruct_tosend.pld_len = ret;
        if(send(sock_fd, &msgstruct_tosend, sizeof(struct message), 0) == -1) {
            perror("Error while sending the file structure message\n");
            return 0;
        }
        if(send(sock_fd, msg_tosend, msgstruct_tosend.pld_len, 0) == -1) {
            perror("Error while sending the file datas");
            return 0;
        }
        //printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct_tosend.pld_len, msgstruct_tosend.nick_sender, msg_type_str[msgstruct_tosend.type], msgstruct_tosend.infos);
        memset(msg_tosend, 0, MSG_LEN);
        memset(&msgstruct_tosend, 0, sizeof(struct message));
    }

    memset(&msgstruct_tosend, 0, sizeof(struct message));
    memset(msg_tosend, 0, MSG_LEN);


    close(fd_in); //close file
    printf("The file has been successfully sent\n");

    return 1;
}

int receive_file(char *name, int sock_fd, char * filename) {
    struct message msgstruct;
    char buff[MSG_LEN];
    memset(&msgstruct, 0, sizeof(struct message));
    memset(buff, 0, MSG_LEN);

    char * filename2 = malloc(sizeof(*filename2));

    struct sockaddr_in client_addr;
    socklen_t size_addr = sizeof(struct sockaddr_in);
    int client_fd = accept(sock_fd,(struct sockaddr*)&client_addr,&size_addr);

    /* create inbox folder if it does not exist already */
    /* save the file in the inbox folder */
    strcat(filename2, "./inbox/");
    strcat(filename2, filename);
    printf("file name : %s\n",filename2);

    printf("Receiving the file...\n");

    int fd_out;
    if(-1 == (fd_out = open(filename2,O_RDWR|O_CREAT|O_TRUNC, 0666))){
        perror("Error on opening the file\n");
    }

    /* receive the datas of the file from the client */
    int ret = -1;
    while(ret != 0){


        // Receiving structure
        if (read(client_fd, &msgstruct, sizeof(struct message)) == -1) {
            printf("Error while receiving the file structure message\n");
            return 0;
        }
        // Receiving message
        if ((ret = read(client_fd, buff, MSG_LEN)) == -1) {
            printf("Error while receiving the file datas\n");	
            return 0;	
        }

        if(ret == 0)
        break;

        /* writing the datas in the new file */
        int wret = 0;
        if ((wret = write(fd_out, buff, ret)) == -1){
            perror("Error while writing in the file\n");
        }
        
	    //printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);

        memset(buff, 0, MSG_LEN);
        memset(&msgstruct, 0, sizeof(struct message));
    }

    printf("The file %s has been saved in ./src/inbox/%s\n", filename, filename);

    memset(buff, 0, MSG_LEN);
    memset(&msgstruct, 0, sizeof(struct message));
    free(filename2);
    close(fd_out);
    close(sock_fd);
    close(client_fd);
    printf("File transfer done!\n");

    return 1;
}

void file_accepted_preparation(char * buff, char * name, int sock_fd, char * file_sender_nickname, char * filename, char address_ip[]){
    /* if the client answered Y to file request, he send a file accept message 
    with the port number and ip adress which will be used for the P2P connection */
	struct message msgstruct;
	char msg_tosend[MSG_LEN];
	memset(msg_tosend, 0, MSG_LEN);
    memset(&msgstruct, 0, sizeof(struct message));

	msgstruct.type = FILE_ACCEPT;
	strncpy(msgstruct.nick_sender, name, strlen(name));
	strncpy(msgstruct.infos, file_sender_nickname, strlen(file_sender_nickname));
	sprintf(msg_tosend, "%s:8082", address_ip);
	msgstruct.pld_len = strlen(msg_tosend);
	send_msg_to_server(sock_fd, msgstruct, msg_tosend); //alert the other client that he is ready to start the connection

    /* Openning a P2P connection by creating a new socket */
	int fd = peer2peer_connection();
    receive_file(name, fd, filename);// receiving the file from the client
    close(fd);//closing P2P connection

	memset(msg_tosend, 0, MSG_LEN);
    memset(&msgstruct, 0, sizeof(struct message));

    /* Sending ack message */
    printf("Sending ack message... ");
    msgstruct.type = FILE_ACK;
    strcpy(msgstruct.infos,file_sender_nickname);
    sprintf(msg_tosend,"%s has received the file", name);
    msgstruct.pld_len = strlen(msg_tosend);
    strcpy(msgstruct.nick_sender, name);

    if(send(sock_fd, &msgstruct, sizeof(struct message), 0) == -1) {
        perror("Error while sending the file structure message\n");
    }
    if(send(sock_fd, msg_tosend, msgstruct.pld_len, 0) == -1) {
        perror("Error while sending the file datas");
    }
    printf("sent!\n");
    //printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);

}

void file_rejected_preparation(char * buff, char * name, int sock_fd, char * file_sender_nickname){
	/* If the client says N to file request, he alerts the other client 
    that he cancelled the file transfert */
    struct message msgstruct;
	char msg_tosend[MSG_LEN];
	memset(msg_tosend, 0, MSG_LEN);
    memset(&msgstruct, 0, sizeof(struct message));

	msgstruct.type = FILE_REJECT;
	strncpy(msgstruct.nick_sender, name, strlen(name));
	strncpy(msgstruct.infos, file_sender_nickname, strlen(file_sender_nickname));
	strcpy(msg_tosend, " ");
	msgstruct.pld_len = strlen(msg_tosend);
	send_msg_to_server(sock_fd, msgstruct, msg_tosend);
    //printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);

}

int peer2peer_connection(){
	/* Create socket */
	printf("Creating socket for peer2peer connection...\n");
	int client_sock = socket(AF_INET, SOCK_STREAM, 0);

	// create  addr
	char  * addr_ip = "127.0.0.1";
	struct sockaddr_in  client_addr;
	memset(&client_addr, '\0', sizeof(client_addr));
	client_addr.sin_family= AF_INET;
	client_addr.sin_port = htons(8082);
	inet_aton(addr_ip,&(client_addr.sin_addr));
		
	// bind to  addr
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