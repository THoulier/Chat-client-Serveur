CFLAGS=-Wall
#LDFLAGS=-lpthread
OBJFILES=liste_chainee.o channel_list.o functions.o file_functions.o
TARGET = server client 

all: $(TARGET)

server:server.o $(OBJFILES)
client:client.o $(OBJFILES)

clean:
	rm -f $(OBJFILES) $(TARGET) *~

