#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "server.h"
#include "database.h"

// main function
int main() {
  // start
  // vector<CancelOrder> cancelOpenSet;
  // vector<ExecutedOrder> cancelExecutedSet;
  // vector<OpenOrder> queryOpenSet;
  // vector<CancelOrder> queryCancelSet;
  // vector<ExecutedOrder> queryExecutedSet;
  
  Database db;
  db.clearTables();
  db.createTables();
  Server sv(db);
  sv.runServer();

  // bool refresh;
  // cin >> refresh;
  // if (refresh){
  //   db.clearTables();
  //   db.createTables();
  //   pugi::xml_document doc1;
  //   pugi::xml_document doc2;
  //   pugi::xml_document doc3;
  //   doc1.load_file("testxmls/test1.xml");
  //   doc2.load_file("testxmls/test2.xml");
  //   doc3.load_file("testxmls/test3.xml");
  //   cout << do_create(doc1, db) << endl;
  //   cout << do_transactions(doc2, db) << endl;
  //   cout << do_transactions(doc3, db) << endl;
  // }
  // else {
  //   pugi::xml_document doc5;
  //   doc5.load_file("testxmls/test5.xml");
  //   cout << do_transactions(doc5, db) << endl;
  // }
  
  
  //cout << "###### Result ######"<< endl;
  
  //db.createAccount("123456", 50000);
  //db.createPosition("SPY", "123456", 5000);
  //db.createOrder("SPY", "123456", 1000, 1000);
  // db.clearTables();
  // db.createTables();
  // db.createAccount("123456", 1000000);
  // db.createAccount("123457", 1000000);
  // db.createPosition("X", "123456", 5000);
  // db.createPosition("X", "123457", 5000);
  // db.createOrder("X", "123456", 300, 125);
  // db.createOrder("X", "123457", -100, 130);
  // db.createOrder("X", "123456", 200, 127);
  // db.createOrder("X", "123457", -500, 128);
  // db.createOrder("X", "123457", -200, 140);
  // db.createOrder("X", "123456", 400, 125);
  // db.createOrder("X", "123457", -400, 124);
  // db.checkBalance("123456", 1000, 1000);
  // db.checkAmount("123456", 5000, "X");
  //db.executeOrder("123457", "7", "SPY", "3000", "100", "1616833730");
  //db.cancel("123456", "1", cancelOpenSet, cancelExecutedSet);
  //db.query("123456", "1", queryOpenSet, queryCancelSet, queryExecutedSet);
  //db.checkAccountValid("123456");
  //cout << cancelOpenSet.size() << endl;
  //cout << cancelExecutedSet.size() << endl;
  //Server server;
  //server.Run(db);
  //close(out);
  return 0;
}

