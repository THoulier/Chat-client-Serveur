
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


