

struct list_client{
	struct client * first;
};

struct client{
	int fd;
	int port;
	char * adress;
	char * nickname;
    struct client * next;
};


struct list_client * initialisation();
void insertion(struct list_client * list, int fd, int port, char * adress);
void suppression(struct client * client, struct list_client * list_principale);
void display_list(struct list_client * list);

struct client * find_client(int client_fd, struct list_client * list_principal);

