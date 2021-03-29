#include "common.h"
#define SERVER_HOSTNAME "vcm-18172.vm.duke.edu"
#define SERVER_PORT "12345"
#define MAX_THREAD 5
#define BUFF_SIZE 10240

void handler(void * fname){
    char buffer[BUFF_SIZE];
    int server_fd;
    int stat;
    int len;
    struct hostent *server_info;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    // get host name of destination server
    server_info = gethostbyname(SERVER_HOSTNAME);
    if (server_info == NULL) {
        cerr << "Host not found.\n";
        exit(EXIT_FAILURE);
    }
    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    stat = getaddrinfo(SERVER_HOSTNAME, SERVER_PORT, &host_info, &host_info_list);

    // create client end socket
    server_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    int yes = 1;
    stat = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(yes));
    if (server_fd < 0) {
        cerr << "Cannot clear given port.\n";
        exit(EXIT_FAILURE);
    }
    // connect to the server via socket
    stat = connect(server_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (stat < 0) {
        cerr << "Connection failed.\n";
        exit(EXIT_FAILURE);
    }
    char *temp = (char *)fname;
    stringstream ss;
    ifstream input_file(temp);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '"<< temp << "'" << endl;
        exit(EXIT_FAILURE);
    }
    ss << input_file.rdbuf();
    string request = ss.str();
    long xml_len = request.length();
    string prefix = to_string(xml_len) + "\n";
    request = prefix + request;
    len = send(server_fd, request.c_str(), request.length(), 0);
    stat = recv(server_fd, buffer, BUFF_SIZE, 0);
    cout<<buffer<<endl;
}

int main(int argc, char** argv){
    if (argc != 2){
        cerr << "Invalid input count." << endl;
        exit(EXIT_FAILURE);
    }
    vector<thread> threads;
    for (int i=0; i<MAX_THREAD; i++){
        thread th(handler, argv[1]);
        threads.push_back(move(th));
        usleep(5000);
    }
    cout << threads.size() << endl;
    //handler(argv[1]);
    for (auto& t : threads){
        t.join();
    }
    return 0;
}