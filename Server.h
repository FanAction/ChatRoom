#ifndef CHATROOM_H_
#define CHATROOM_H_

#include <string>
#include "Common.h"

using namespace std;

//!The Server To Proces Request
class Server
{
public:
    Server();

    void Init();

    void Close();

    void Start();

private:
    int SendBoardcastMessage(int clientId);

    struct sockaddr_in serverAddr;

    int listener;

    //!The ReVal for epoll_create
    int epfd;

    //!The clients vector
    list<int> clients_list;


};

#endif
