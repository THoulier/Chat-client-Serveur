#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>

#include "liste_chainee.h"


void update_nickname(struct client * client, char * nickname){
    strcpy(client->nickname, nickname);
}

struct client * find_client_nickname(char * nickname, struct list_client * list_principal){
    /* find a client having his nickname */
    struct client * first_client = malloc(sizeof(*first_client));
    first_client = list_principal->first;
	if (strcmp(first_client->nickname,nickname) == 0){
		return 	first_client;	
	}
	else {
		while (first_client != NULL){
			if (strcmp(first_client->nickname,nickname) == 0){
                return 	first_client;
			}
			else{
				first_client=first_client->next;
			}
		}
	}
    return NULL;
}


struct client * find_client(int client_fd, struct list_client * list_principal){
    /* find a client having his fd */
    struct client * first_client = malloc(sizeof(*first_client));
    first_client = list_principal->first;
	if (first_client->fd == client_fd){
		return 	first_client;	
	}
	else {
		while (first_client != NULL){
			if (first_client->fd == client_fd){
                return 	first_client;
			}
			else{
				first_client=first_client->next;
			}
		}
	}
    return NULL;
}

struct list_client * initialisation(){
    /* list initialisation */
    struct list_client * list = malloc(sizeof(*list));
    struct client * client = malloc(sizeof(*client));

    if(list == NULL || client == NULL){
        exit(EXIT_FAILURE);
    }
    
    client->fd = 0;
    client->port = 0;
    client->adress = NULL;
    client->connection_time = NULL;
    strcpy(client->nickname, "");  
    return list;
}

void insertion(struct list_client * list, int fd, int port, char * adress){
    /* insertion of a client in the list */
    struct client * new = malloc(sizeof(*new));
    struct tm *tmp = malloc(sizeof(*tmp));
    if(list == NULL || new == NULL){
        exit(EXIT_FAILURE);
    }
    time_t current_time = time(NULL);
    new->connection_time = ctime(&current_time);

    new->fd = fd;
    new->port = port;
    new->adress = adress;
    strcpy(new->nickname, "");
    new->next = list->first;
    list->first = new;
}

void suppression(struct client *client, struct list_client *list_principale){
    /* suppression of a client in the list */
	struct client * prev = malloc(sizeof(*prev));
	struct client * next = malloc(sizeof(*next));
	struct client * first_client = malloc(sizeof(*first_client));
    
    first_client = list_principale->first;

	if (first_client == client){
        if(list_principale->first->next == NULL){
            list_principale->first = NULL;
        }
        else{
            list_principale->first = list_principale->first->next;

        }
	}
    
	else{
		while (first_client != NULL){
			if (first_client->next == client){
				prev = first_client;
                first_client = first_client->next;
				next = first_client->next;
				first_client = NULL;
			}
			else {
				first_client = first_client->next;
			}
		}
		prev->next = next;
	}
	free(client);
}

void display_list(struct list_client * list){
    if (list == NULL){
        exit(EXIT_FAILURE);
    }
    struct client * current = malloc(sizeof(*current));
    current = list->first;
    printf("list of client fd :");
    while (current != NULL){
        printf("%d ->",current->fd);
        current = current->next;
    }
    printf("NULL\n");

    current = list->first;
    printf("list of client adress :");
    while (current != NULL){
        printf("%s ->",current->adress);
        current = current->next;
    }
    printf("NULL\n");

    current = list->first;
    printf("list of client port number :");
    while (current != NULL){
        printf("%d ->",current->port);
        current = current->next;
    }
    printf("NULL\n");

    current = list->first;
    printf("list of client nickname :");
    while (current != NULL){
        printf("%s ->",current->nickname);
        current = current->next;
    }
    printf("NULL\n");

    current = list->first;
    printf("list of client time :");
    while (current != NULL){
        printf("%s ->",current->connection_time);
        current = current->next;
    }
    printf("NULL\n");

}
