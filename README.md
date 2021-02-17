# Client/Server Chat

Installation
------------
To install the application, you have to clone the repo and compile the files :

    $ git clone https://github.com/THoulier/Chat-client-serveur.git
    $ cd src
    $ make
    
First, you have to launch the server in one terminal :

    $ ./server <server_port>

Then, you can connect a client to the server in another terminal :

    $ ./client <server_ip_adresse> <server_port>
    
A client corresponds to a terminal, you can create as many clients as you want.


Features
------------
### Create a user
```
Connecting to server ... done!
[Server] : please login with /nick <your pseudo>
/nick CoolestUserEver
[Server] : Welcome on the chat CoolestUserEver
```

### Private/Public messages
Public messages are sent to every users connected to the TCP server :
```
$ terminal_user0>  /msgall Hello all
$ terminal_user1 > [user0] : Hello all
$ terminal_user2 > [user0] : Hello all
```
Private messages are sent to the referenced user :
```
$ terminal_user0> /msg user1 Hello
$ terminal_user1 > [user0] : Hello
```

### Channel
To create and join a chennel :
```
$ terminal_user0> /create best_channel
$ terminal_user0> You have created channel best_channel
$ terminal_user0[best_channel]> You have joined best_channel

$ terminal_user1> /join best_channel
$ terminal_user1[best_channel]> You have joined best_channel
```
Example of a conversation in a channel :
```
$ terminal_user0[best_channel]>  I'm downtown
$ terminal_user0[best_channel]> [Me] : I'm downtown
$ terminal_user1[best_channel]> [user0] : I'm downtown
$ terminal_user0[best_channel] > /quit best_channel
$ terminal_user1[best_channel] > user0 has quit best_channel
$ terminal_user1[best_channel] > /quit best_channel
$ terminal_user1[best_channel] > You have quit best_channel
```
When the channel is empty, it is destroyed :
```
$ terminal_user1> best_channel has been destroyed
```
### File transfert
### Information commands

Screenshots
------------
