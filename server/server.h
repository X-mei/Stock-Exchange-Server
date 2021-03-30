#ifndef SERVER_H
#define SERVER_H
#include "common.h"
#include "parser.h"
#define PORT "12345"
#define BUFF_SIZE 409600

class Server{
private:
    int socket_fd;
    int status;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    void recvRequest(int new_socket_fd);
    vector<char> recv_vector(int new_socket_fd);
    void send_back(int new_socket_fd, string &response);
public:
    Server();
    void runServer();
    ~Server() {}

};

#endif