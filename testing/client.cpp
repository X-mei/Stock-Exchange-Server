#include "common.h"

void handler(){
    char buffer[BUFF_SIZE];
    int server_sfd;
    int server_port_num;
    int stat;
    int len;
    struct hostent *server_info;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;

    server_info = gethostbyname(SERVER_ADDR);
    if (server_info == NULL) {
        std::cerr << "host not found\n";
        exit(1);
    }
    server_port_num = 12345; // atoi(SERVER_PORT);

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    stat = getaddrinfo(SERVER_ADDR, SERVER_PORT, &host_info, &host_info_list);

    // create socket
    server_sfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    int yes = 1;
    stat = setsockopt(server_sfd, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(yes));
    if (server_sfd < 0) {
        perror("socket");
        exit(server_sfd);
    }
    // connect to the server
    stat = connect(server_sfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (stat < 0) {
        perror("server connect");
        exit(stat);
    }
}