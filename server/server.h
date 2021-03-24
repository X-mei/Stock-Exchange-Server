#ifndef SERVER_H
#define SERVER_H
#include "common.h"

#define PORT "12345"

class Server{
private:
    int socket_fd;
    //int status;
    //socklen_t sin_size;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    struct sockaddr_storage their_addr;
    
public:
    Server();
    void runServer();
    ~Server();

};

#endif