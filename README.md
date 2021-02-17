# Client/Server Chat

A local chat application using a multitask TCP server. 
The main features implemented are :
- private/public messages
- file transfert
- channel

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
$ terminal_user0 >  /msgall Hello all
$ terminal_user1 > [user0] : Hello all
$ terminal_user2 > [user0] : Hello all
```
Private messages are sent to the referenced user :
```
$ terminal_user0 > /msg user1 Hello
$ terminal_user1 > [user0] : Hello
```

### Channel
To create and join a channel :
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
You can transfert a file between two clients. The file is sent by 1024 bits packages.
```
$ terminal_user1> /send user2 "/home/user/file.txt"
                            $ terminal_user2> user1 wants you to accept the
                            transfer of the file named "file.txt". Do you
                            accept? [Y/N]
                            $ terminal_user2> Y
$ terminal_user1> user2 accepted file transfert.
$ terminal_user1> Connecting to user2 and sending the file...
                            $ terminal_user2> Receiving the file from 
                            user1...
                            $ terminal_user2> file.txt saved in 
                            .src/inbox/file.txt
$ terminal_user1> user2 has received the file.

$ terminal_user1> /send user2 "/home/user/correction_du_projet.txt"
                            $ terminal_user2> user1 wants you to accept the 
                            transfer of the file named 
                            "correction_du_projet.txt". Do you accept? 
                            [Y/N]
                            $ terminal_user2> N
$ terminal_user1> user2 cancelled file transfer.
```
### Information commands
To see the users connected :
```
/who
[Server] : Online users are
                          - User1
                          - User2
                          - CoolestUserEver

```
To see personnal informations on a specific user :
```
/whois  User1
[Server] : User1 connected since 2014/09/29@19:23 with IP address 192.168.3.165 and port number 52322
```
To see the list of current channels :
```
/channel_list
[Server] : Online channels are :
                          - Best_channel
                          - channel_2
                          - test_channel

```


