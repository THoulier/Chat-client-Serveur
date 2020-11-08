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

#include "channel_list.h"


struct channel * find_channel_name(char *name, struct channel_list * list_principal){
    /* find a client having his nickname */
    struct channel * first_channel = malloc(sizeof(*first_channel));
    first_channel = list_principal->first;
	if (strcmp(first_channel->name,name) == 0){
		return 	first_channel;	
	}
	else {
		while (first_channel != NULL){
			if (strcmp(first_channel->name,name) == 0){
                return 	first_channel;
			}
			else{
				first_channel=first_channel->next;
			}
		}
	}
    return NULL;
}

struct channel_list * channel_initialisation(){
    /* list initialisation */
    struct channel_list * list = malloc(sizeof(*list));
    struct channel * channel = malloc(sizeof(*channel));

    if(list == NULL || channel == NULL){
        exit(EXIT_FAILURE);
    }
    memset(channel->fds,-1,MAXCLI*sizeof(int));
    memset(channel->name,0,NICK_LEN);
 
    return list;
}

void channel_insertion(struct channel_list * list, int fd, char * name){
    /* insertion of a client in the list */
    struct channel * new = malloc(sizeof(*new));
    if(list == NULL || new == NULL){
        exit(EXIT_FAILURE);
    }
    memset(new->fds,-1,MAXCLI*sizeof(int));
    memset(new->name,0,NICK_LEN);
    new->fds[0] = fd;
    strcpy(new->name,name);;

    new->next = list->first;
    list->first = new;
}

void channel_suppression(struct channel *channel, struct channel_list *list_principale){
    /* suppression of a client in the list */
	struct channel * prev = malloc(sizeof(*prev));
	struct channel * next = malloc(sizeof(*next));
	struct channel * first_channel = malloc(sizeof(*first_channel));
    
    first_channel = list_principale->first;

	if (first_channel == channel){
        if(list_principale->first->next == NULL){
            list_principale->first = NULL;
        }
        else{
            list_principale->first = list_principale->first->next;

        }
	}
    
	else{
		while (first_channel != NULL){
			if (first_channel->next == channel){
				prev = first_channel;
                first_channel = first_channel->next;
				next = first_channel->next;
				first_channel = NULL;
			}
			else {
				first_channel = first_channel->next;
			}
		}
		prev->next = next;
	}
	free(channel);
}


void display_channel_list(struct channel_list * list){
    if (list == NULL){
        exit(EXIT_FAILURE);
    }
    struct channel * current = malloc(sizeof(*current));
    current = list->first;
    printf("list of channel fds :");
    while (current != NULL){
        for (int i=0; i<MAXCLI; i++){
            if (current->fds[i] != -1){
                printf("%d ->",current->fds[i]);
            }
        }
        current = current->next;
    }
    printf("NULL\n");

    current = list->first;
    printf("list of channel names :");
    while (current != NULL){
        printf("%s ->",current->name);
        current = current->next;
    }
    printf("NULL\n");
}

