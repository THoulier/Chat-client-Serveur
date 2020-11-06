#include "common.h"

struct list_client * list_client;

struct list_client{
	struct client * first;
};

struct client{
	int fd;
	int port;
	char * adress;
	char nickname[MSG_LEN];
    struct client * next;
	char * connection_time;
};


struct list_client * initialisation();
void insertion(struct list_client * list, int fd, int port, char * adress);
void suppression(struct client * client, struct list_client * list_principale);
void display_list(struct list_client * list);

struct client * find_client(int client_fd, struct list_client * list_principal);
void update_nickname(struct client * client, char nickname[MSG_LEN]);

struct client * find_client_nickname(char * nickname, struct list_client * list_principal);