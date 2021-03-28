#include "server.h"
#define BUFF_SIZE 409600

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

vector<char> recv_vector(){
    vector<char> buff(BUFF_SIZE);
    int data_len = recv(socket_fd, &(buff.data()[0]), BUFF_SIZE, 0);
    int index = data_len;
    int total_len = 0;
    try {
        total_len = stoi(string(buff.data()).substr(0, string(buff.data()).find('\n') + 1));
    } 
    catch (const exception &e) {
        return {};
    }

    cout << "=========================" << endl;
    cout << "starting receiving" << endl;
    cout << "data_len: " << data_len << endl;
    cout << "total_len: " << total_len << endl;
    if (data_len >= BUFF_SIZE) {
        while (data_len != 0) {
            buff.resize(index + 1024);
            data_len = recv(socket_fd, &(buff.data()[index]), 1024, 0);
            index += data_len;
            if (data_len < 1024 && data_len > 0) {
                buff.resize(index);
            }
            if (data_len <= 0 || total_len <= index) {
                break;
            }
            if (string(buff.begin(), buff.end()).find("</create>") != string::npos) {
                break;
            }
        cout << buff.data() << endl;
        }
    }

    cout << "done receiving" << endl;
    // get rid of byte len at the beginning
    while (isdigit(*buff.begin())) {
        buff.erase(buff.begin());
    }
    buff.erase(buff.begin());
    return buff;
    }
}

void send_back(int &client_fd, string &response) {
  cout << "start sending back" << endl;
  size_t sent = 0;
  vector<char> res(response.begin(), response.end());
  while (1) {
    if (sent + 1024 < res.size()) {
      sent += send(client_fd, &(res.data()[sent]), 1024, 0);
    } else {
      sent += send(client_fd, &(res.data()[sent]), res.size() - sent, 0);
      break;
    }
  }
  cout << "done sending back" << endl;
  return;
}

void Server::runServer(){
    
}