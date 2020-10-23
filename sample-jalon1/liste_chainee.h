
struct list_client{
	struct client * first;
};

struct client{
	int fd;
	int port;
	char * adress;
    struct client * next;
};


struct list_client * initialisation();
void insertion(struct list_client * list, int fd, int port, char * adress);
void suppression(struct list_client * list);
void display_list(struct list_client * list);









/*
struct list_client_datas{
	struct client_datas * client;
	struct list_client_datas * next;
};

struct client_datas{
	int fd;
	int port;
	char * adress;
};

void client_sup(struct client_datas * client);
void client_insertion(int fd, int port, char * adress);
*/