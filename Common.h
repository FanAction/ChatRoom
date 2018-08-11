#ifndef CHATROOM_H_
#define CHATROOM_H_

#include <iostream>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//!default server Ip addr
#define SERVER_IP "127.0.0.1"

//!default server port
#define SERVER_PORT 8888

//!the epoll MaxSize
#define EPOLL_SIZE 500

//!the buffer size
#define BUF_SIZE OXFFFF

//!the welcome string for new client
#define SERVER_WELCOME "Welcome you join to the char room! Your chart ID is: Client: %d"

//!other client's message prefer
#define SERVER_MESSAGE "ClientID %d Say >> %s"

//!Exit the system
#define EXIT "EXIT"

//!Notify you are the only custemer is the chatroom
#define CAUTION "There is only one int chat room"

/**
 * @purpose: Register the new fd to epoll
 * @param:   epollfd - the created epollfd
 * @param:   fd      - the file descrip
 * @param:   enable_et - whether use ET Mode
 *
 * @return:  void
 * **/
static void addfd(int epollfd, int fd, bool enable_et)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if(enable_et)
    {
        ev.events = EPOLLIN | EPOLLET;
    }
    epoll_ctl(epollfd, EPOLL_CLT_ADD, fd, &ev);
    //!Set Socket the Nonblocking Mode
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);

    printf("fd added to epoll!\n\n");
}

#endif
