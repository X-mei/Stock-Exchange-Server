#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
//#include <signal.h>

#include <ctime>
#include <fstream>
#include <iostream>
#include <pqxx/pqxx>
#include <vector>
#include <mutex>

#include "pugixml/src/pugixml.hpp"

using namespace std;

class OpenOrder{
public:
    float shares;
    OpenOrder(int s):shares(s){};
};

class CancelOrder{
public:
    float shares;
    time_t time;
    CancelOrder(int s, time_t t):shares(s), time(t){};
};

class ExecutedOrder{
public:
    float prices;
    float shares;
    time_t time;
    ExecutedOrder(float s, float a, time_t t):prices(a), shares(s), time(t){};

};

#endif