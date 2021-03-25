#ifndef DATABASE_H
#define DATABASE_H

#include "common.h"
#include "myexception.h"
using namespace std;


class Database{
public:
    pqxx::connection *C;
    Database();
    ~Database();
    //void setup();
    void clearTables();
    void createTables();
    bool createAccount(string accountId, float balance);    //false for account exist, true for create new account
    //false if position is invalid, true of position is created or updated
    bool createPosition(string symName, string accountId, float amount);
    bool createOrder(string symName, string accountId, float amount, float price);


};


#endif