#include "common.h"
#define NICK_LEN 128

struct channel_list * channel_list;

struct channel_list{
	struct channel * first;
};

struct channel{
    int fds[MAXCLI];
    char name[NICK_LEN];
    struct channel * next;
};


struct channel_list * channel_initialisation();
void channel_insertion(struct channel_list * list, int fd, char * name);
void channel_suppression(struct channel *channel, struct channel_list *list_principale);
void display_channel_list(struct channel_list * list);
struct channel * find_channel_name(char *name, struct channel_list * list_principal);