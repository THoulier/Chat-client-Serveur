#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include "liste_chainee.h"




struct list_client * initialisation(){
    struct list_client * list = malloc(sizeof(list));
    struct client * client = malloc(sizeof(client));

    if(list == NULL || client == NULL){
        exit(EXIT_FAILURE);
    }
    client->fd = 0;
    client->port = 0;
    client->adress = NULL;
    
    return list;
}

void insertion(struct list_client * list, int fd, int port, char * adress){
    struct client * new = malloc(sizeof(new));
    if(list == NULL || new == NULL){
        exit(EXIT_FAILURE);
    }
    new->fd = fd;
    new->port = port;
    new->adress = adress;

    new->next = list->first;
    list->first = new;
}

void suppression(struct list_client * list){
    if(list == NULL){
        exit(EXIT_FAILURE);
    }
    if(list->first != NULL){
        struct client * to_sup = list->first;
        list->first = list->first->next;
        free(to_sup);
    }
}

void display_list(struct list_client * list){
    if (list == NULL){
        exit(EXIT_FAILURE);
    }
    struct client * current = list->first;
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
}
