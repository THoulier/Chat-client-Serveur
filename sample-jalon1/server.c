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

void echo_server(int server_sock) {

	/*Init of poll structure */
	int nfds = MAXCLI;
	struct pollfd fds[nfds];
	memset(fds,0,nfds*sizeof(struct pollfd));

	for (int p =0; p<nfds;p++){
		fds[p].fd = -1;
	}
	fds[0].fd = server_sock;
	fds[0].events = POLLIN;

	while (1) {
		/* tests on poll function */
		int enabled = 0;
        enabled = poll(fds,nfds,-1);

        if (enabled >0){
            printf("There is something to do\n");
        }
        else{
            printf("There is a problem with poll function: error %i\n",enabled);
            continue;
        }
        
        for (int i = 0; i < nfds; i++){
			/* connection treated with poll function */
            if ( fds[i].revents == POLLIN && i == 0){

				int fd = 1;
                struct sockaddr_in client_addr;
                socklen_t size_addr = sizeof(struct sockaddr_in);
                int client_fd = accept(server_sock,(struct sockaddr*)&client_addr,&size_addr);

                while(fds[fd].fd != -1 && fd < nfds){
                    fd++;    
				}
                if(fd < nfds){
                    fds[fd].fd = client_fd;
                    fds[fd].events = POLLIN;
                    printf("Client socket %i accepted and assigned to fd %i\n", client_fd, fd);
                }
				else if (fd < nfds){
                    perror("The max number of connections is reached\n");
                    close(client_fd);
                }
                fds[i].revents = 0;
            }
            
            else if ((fds[i].revents & POLLHUP )&& i != 0){
                close(fds[i].fd);
                printf("Socket %i has terminated its connection\n",i);
                memset((fds + i),0,sizeof(struct pollfd));
            }

            else if (fds[i].revents == POLLIN && i != 0){
                char buffer[MSG_LEN];
                memset(buffer,0,MSG_LEN);
                int ret = 0;
                int tmp = 0;
                
                if (-1 == ( ret = read(fds[i].fd,buffer,MSG_LEN))) {
                    perror("Error while reading");
                    break;
                }
				if(ret != -1){
        			write(fds[i].fd, buffer, ret);
        			printf("The buffer received by client %i is %s\n", fds[i].fd,buffer);
    			}
    			memset(buffer, '\0', MSG_LEN);
            }    
        }
		/*
		// Cleaning memory
		memset(buff, 0, MSG_LEN);
		// Receiving message
		if (recv(sockfd, buff, MSG_LEN, 0) <= 0) {
			break;
		}
		printf("Received: %s", buff);
		// Sending message (ECHO)
		if (send(sockfd, buff, strlen(buff), 0) <= 0) {
			break;
		}
		printf("Message sent!\n");*/
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
	int sfd;
	sfd = handle_bind(atoi(argv[1]));

	echo_server(sfd);
	close(sfd);
	
	return EXIT_SUCCESS;
}

