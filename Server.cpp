#include <iostream>
#include "Server.h"

using namespace std;

Server::Server()
{
    //!Init Server Addr and port
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    //!Init socket
    listener = 0;

    //!epool fd
    epfd = 0;
}

//!Init Server and Start Listening
void Server::Init()
{
    cout << "Init Server.." << endl;

    //create socket
    listener = socket(PF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        perror("listener");
        exit(-1);
    }

    //bind the addr
    if(bind(listener, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("bind error");
        exit(-1);
    }

    //Start Listenning
    int ret = listen(listener, 5);
    if(ret < 0)
    {
        perror("listern error");
        exit(-1);
    }

    cout << "Start to Listen:" << SERVER_IP << endl;

    //Create EventTable in core
    epfd = epoll_create(EPOLL_SIZE);
    if(epdf < 0)
    {
        perror("epdf error");
        exit(-1);
    }

    //Add Listen Event to EventTable
    addfd(epfd, listener, true);
}

void Server::Close()
{
    //close socket
    close(listener);

    //close epoll monitor
    close(epfd);
}

int Server::SendBroadcastMessage(int clientfd)
{
    //buf[BUF_SIZE]     the new received message
    //message[BUF_SIZE] the formatted message
    char buf[BUF_SIZE], message[BUF_SIZE];
    bzero(buf, BUF_SIZE);
    bzero(message, BUF_SIZE);

    //Receive the new Message
    cout << "read from client(clientID = )" << clientfd << ")" << endl;
    int len = recv(clientfd, buf, BUF_SIZE, 0);

    //if the Client is not exits
    if(len == 0)
    {
        close(clientfd);

        //remove the clientfd from list
        clients_list.remove(clientfd);
        cout << "ClientID=" << clientfd << "closed.\n now there are" << clients_list.size() << "client in the chat room" << endl;
    }
    //Send message to all the clients
    else
    {
        if(clients_list.size()==1)
        {
            //Send caution message
            send(clientfd, CAUTION, strlen(CAUTION), 0);
            return len;
        }

        //Format the message
        sprintf(message, SERVER_MESSAGE, clientfd, buf);

        //Send message to other clients
        list<int>::iterator it;
        for(it = clients_list.begin(); it != clients_list.end(); ++it)
        {
            if(*it != clientfd)
            {
                if(send(*it, message, BUF_SIZE, 0) < 0)
                {
                    return -1;
                }
            }
        }
    }
    return len;
}

void Server::Start()
{
    //epoll evnet queue
    static struct epoll_event events[EPOLL_SIZE];

    //Init the server
    Init();

    while(1)
    {
        int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);

        if(epoll_events_count<0)
        {
            perror("epoll failure");
            break;
        }

        cout << "epoll_events_count = \n" << epoll_events_count << endl;

        //process the epoll events
        for(int i = 0; i<epoll_events_count; ++i)
        {
            int sockfd = events[i].data.fd;
            //new user connect
            if(sockfd == listener)
            {
                struct sockaddr_in client_addr;
                socklen_t client_addrLength = sizeof(struct sockaddr_in);
                int clientfd = accept(listener, (struct sockaddr*)&client_addr, &client_addrLength);

                cout << "client connection from:"
                     << inet_ntoa(client_address.sin_addr) << ":"
                     << ntohs(client_address.sin_port) << ",clientfd = "
                     << clientfd << endl;

                addfd(epfd, clientfd, true);

                //Save client to the list
                clients_list.push_back(clientfd);
                cout << "Add new clientfd = " << clientfd << "to epoll" << endl;
                cout << "Now there are "<< clients_list.size() << "clients in the chat room";

                //send welcom to the clients
                cout << "welcome message" << endl;
                char message[BUF_SIZE];
                bzero(message, BUF_SIZE);
                sprintf(message, SERVER_WELCOME, clientfd);
                int ret = send(clientfd, message, BUF_SIZE, 0);
                if(ret < 0)
                {
                    perror("send error");
                    Close();
                    exit(-1);
                }
            }
            else
            {
                int ret = SendBroadcastMessage(sockfd);
                if(ret < 0)
                {
                    perror("error");
                    Close();
                    exit(-1);
                }
            }
        }
    }
    //close the server
    Close();
}
