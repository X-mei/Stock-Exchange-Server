#include <stdio.h>
#include <stdlib.h>

#include "database.h"

// main function
int main() {
  // start
  vector<CancelOrder> cancelOpenSet;
  vector<ExecutedOrder> cancelExecutedSet;
  vector<OpenOrder> queryOpenSet;
   vector<CancelOrder> queryCancelSet;
  vector<ExecutedOrder> queryExecutedSet;
 
  Database db;
  db.clearTables();
  db.createTables();
  db.createAccount("123456", 50000);
  db.createPosition("SPY", "123456", 5000);
  db.createOrder("SPY", "123456", 1000, 1000);
  db.cancel("123456", "1", cancelOpenSet, cancelExecutedSet);
  db.query("123456", "1", queryOpenSet, queryCancelSet, queryExecutedSet);
  db.checkAccountValid("123456");
  //cout << cancelOpenSet.size() << endl;
  //cout << cancelExecutedSet.size() << endl;
  //Server server;
  //server.Run(db);
  // close(out);
  return 0;
}