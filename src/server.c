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

#include "functions.h"
#include "liste_chainee.h"
#include "common.h"
#include "channel_list.h"



void connection_client(int client_nb, int nfds, int server_sock, struct pollfd * fds) {
	/* treats the connection of a client */
	int fd = 1;
    struct sockaddr_in client_addr;
    socklen_t size_addr = sizeof(struct sockaddr_in);
    int client_fd = accept(server_sock,(struct sockaddr*)&client_addr,&size_addr);
	char * client_ip = inet_ntoa(client_addr.sin_addr);
	
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
	/*treats the disconnection of a client */
	suppression(find_client(client_fd,list_client),list_client); //remove the client from the list
	close(client_fd);
	printf("[Client %i] disconnected\n",client_nb);
	memset((fds + client_nb),0,sizeof(struct pollfd));
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
		//display_list(list_client);
		display_channel_list(channel_list);
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
	channel_list = channel_initialisation();

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

