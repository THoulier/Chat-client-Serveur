CFLAGS=-Wall
#LDFLAGS=-lpthread
OBJFILES=liste_chainee.o
TARGET = server client

all: $(TARGET)

server:server.o $(OBJFILES)
client:client.o $(OBJFILES)

clean:
	rm -f $(OBJFILES) $(TARGET) *~

