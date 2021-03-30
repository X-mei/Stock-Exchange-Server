#include "common.h"
#define SERVER_HOSTNAME "vcm-18172.vm.duke.edu"
#define SERVER_PORT "12345"
#define MAX_THREAD  1000
#define BUFF_SIZE 10240

void handler(void * fname, vector<float>& runTimes, int i){
    clock_t start,end;
    start = clock();
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
    free(host_info_list);
    close(server_fd);
    //cout<<buffer<<endl;
    float timeSpan = (float)(start-end)*1000/CLOCKS_PER_SEC;
    runTimes[i] = timeSpan;
    //cout << "Total time: " << timeSpan << endl;
    end = clock();
}

int main(int argc, char** argv){
    if (argc != 2){
        cerr << "Invalid input count." << endl;
        exit(EXIT_FAILURE);
    }
    vector<thread> threads;
    vector<float> runTimes(MAX_THREAD);
    for (int i=0; i<MAX_THREAD; i++){
        thread th(handler, argv[1], ref(runTimes), i);
        threads.push_back(move(th));
        usleep(500);
    }
    for (auto& t : threads){
        t.join();
    }
    for (int i=0; i<MAX_THREAD; i++){
        cout << "Run time for thread " << i << ": " << runTimes[i] << endl;
    }
    return 0;
}