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
    int createOrder(string symName, string accountId, float amount, float limit);
    bool executeOrder(string accountId, string transId, string symName, float amount, float limit, time_t orderTime);
    void updateSingleOrder(string accountId, string transId, string symName, float amount, float limit, time_t orderTime);
    void deleteSingleOrder(string accountId, string transId, string symName, float limit, time_t orderTime);
    void processSingleTrade(string buyAccountId, string sellAccountId, string buyTransId, string sellTransId, string symName, float amount, float price);
    bool cancel(string accountId, string transId, vector<CancelOrder> &cancelOpenSet, vector<ExecutedOrder> &cancelExecutedSet);
    bool query(string accountId, string transId, vector<OpenOrder> &queryOpenSet, vector<CancelOrder> &queryCancelSet, vector<ExecutedOrder> &queryExecutedSet);
    bool checkAccountValid(string accountId);
    bool checkBalance(string accountId, float amount, float limits);
    bool checkAmount(string accountId, float amount, string symName);
};

#endif