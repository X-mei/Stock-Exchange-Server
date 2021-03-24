#include "server.h"

Server::Server(){
    memset(&host, 0, sizeof(host));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;
    status = getaddrinfo(NULL, SERVERPORT, &host, &host_info_list);

    if (status != 0) {
        cerr << "Error: address issue" << endl;
        exit(EXIT_FAILURE);
    }

    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);

    if (socket_fd == -1) {
        cerr << "Error: socket creation failed" << endl;
        exit(EXIT_FAILURE);
    }

    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (status == -1) {
        cerr << "Error: socket operation fail" << endl;
        exit(EXIT_FAILURE);
    }

    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: Binding fail" << endl;
        exit(EXIT_FAILURE);
    }

    status = listen(socket_fd, 10240);
    if (status == -1) {
        cerr << "Error: listen fail" << endl;
        exit(EXIT_FAILURE);
    }
}