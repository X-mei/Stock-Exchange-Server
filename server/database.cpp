#include "database.h"
using namespace std;

Database::Database(){
    try{
        C = new pqxx::connection("dbname=exchangedb user=postgres password=Z hostaddr=127.0.0.1 port=5432");
        //this->C = temp;
      //C = new pqxx::connection("dbname=testdb user=postgres password=cohondob, hostaddr=127.0.0.1 port=5432");
        if (C->is_open()) {
            cout << "Opened database successfully: " << C->dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
            return;
        }
    }
    catch(const exception &e){
        cerr << e.what() << endl;
        return;
    }
}
Database::~Database(){
    C -> disconnect();
}

// void Database::setup(){
//     try{
//         C = new pqxx::connection("dbname=testdb user=postgres password=cohondob hostaddr=127.0.0.1 port=5432");
//         //this->C = temp;
//       //C = new pqxx::connection("dbname=testdb user=postgres password=cohondob, hostaddr=127.0.0.1 port=5432");
//         if (C->is_open()) {
//             cout << "Opened database successfully: " << C.dbname() << endl;
//         } else {
//             cout << "Can't open database" << endl;
//             return;
//         }
//     }
//     catch(const exception &e){
//         cerr << e.what() << endl;
//         return;
//     }
// }

void Database::clearTables(){
    try{
        string drop_table_sql = "DROP TABLE ACCOUNT, POSITIONS, ORDER_OPEN, ORDER_EXECUTION, ORDER_CANCEL;";
        pqxx::work W(*C);
        W.exec(drop_table_sql);
        W.commit();
        cout << "Drop all previous tables successfully" << endl;
    }
    catch(const exception &e){
        cerr << e.what() << endl;
        return;
    }
}

void Database::createTables() {
    pqxx::work W(*C);

    std::string account_sql = "CREATE TABLE ACCOUNT("\
    "ACCOUNT_ID INT PRIMARY KEY NOT NULL UNIQUE,"\
    "BALANCE FLOAT NOT NULL);";
    W.exec(account_sql);
    //cout << "create ACCOUNT successfully" << endl;
  
    std::string position_sql =
    "CREATE TABLE POSITIONS("\
    "id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY NOT NULL,"\
    "ACCOUNT_ID INT NOT NULL,"\
    "FOREIGN KEY(ACCOUNT_ID) REFERENCES ACCOUNT(ACCOUNT_ID) ,"\
    "SYM VARCHAR(50) NOT NULL,"\
    "AMOUNT FLOAT NOT NULL);";
    W.exec(position_sql);
    //cout << "create POSITIONS successfully" << endl;
  
    std::string order_sql =
    "CREATE TABLE ORDER_OPEN("\
    "id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY  NOT NULL,"\
    "ACCOUNT_ID INT NOT NULL,"\
    "FOREIGN KEY(ACCOUNT_ID) REFERENCES ACCOUNT(ACCOUNT_ID),"\
    "SYM VARCHAR(50) NOT NULL,"\
    "AMOUNT FLOAT NOT NULL,"\
    "LIMITS FLOAT NOT NULL,"\
    "TIME BIGINT NOT NULL);";
    W.exec(order_sql);
    //cout << "create ORDER_OPEN successfully" << endl;
  
    std::string execution_sql =
    "CREATE TABLE ORDER_EXECUTION("\
    "id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY NOT NULL,"\
    "BUYER_ID INT NOT NULL,"\
    "SELLER_ID INT NOT NULL,"\
    "BUY_ID INT NOT NULL,"\
    "SELL_ID INT NOT NULL,"\
    "FOREIGN KEY(BUYER_ID) REFERENCES ACCOUNT(ACCOUNT_ID),"	\
    "FOREIGN KEY(SELLER_ID) REFERENCES ACCOUNT(ACCOUNT_ID),"\
    /*"FOREIGN KEY(BUY_ID) REFERENCES ORDER_OPEN(id),"\*/
    /*"FOREIGN KEY(SELL_ID) REFERENCES ORDER_OPEN(id),"\*/
    "SYM VARCHAR(50) NOT NULL,"\
    "PRICE FLOAT NOT NULL,"	\
    "AMOUNT FLOAT NOT NULL,"\
    "TIME BIGINT NOT NULL);";
    W.exec(execution_sql);
    //cout << "create ORDER_EXECUTION successfully" << endl;

    std::string cancel_sql =
    "CREATE TABLE ORDER_CANCEL("\
    "id INT GENERATED ALWAYS AS IDENTITY PRIMARY KEY NOT NULL,"\
    "ACCOUNT_ID INT NOT NULL,"\
    "TRANS_ID INT NOT NULL,"\
    "FOREIGN KEY(ACCOUNT_ID) REFERENCES ACCOUNT(ACCOUNT_ID),"	\
    /*"FOREIGN KEY(TRANS_ID) REFERENCES ORDER_OPEN(id),"	\*/
    "AMOUNT FLOAT NOT NULL,"\
    "TIME BIGINT NOT NULL);";
    W.exec(cancel_sql);
    //cout << "create ORDER_CANCEL successfully" << endl;

    W.commit();
    cout << "create all tables successfully" << endl;
}

bool Database::createAccount(string accountId, float balance){
    try{
        string select_account_sql = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID = " + accountId + ";";
        pqxx::nontransaction N(*C);
        //pqxx::work N(*C);
        pqxx::result R( N.exec(select_account_sql));
        pqxx::result::const_iterator c = R.begin(); 
        if( c != R.end()){     
            return false;
        }
        N.commit();
    /*}
    catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }*/
    string create_new_accoun_sql = "INSERT INTO ACCOUNT (ACCOUNT_ID, BALANCE) VALUES (" + accountId + ", " + to_string(balance) + ");";
    pqxx::work W(*C);
    W.exec(create_new_accoun_sql);
    W.commit();
    return true;
    }
    catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

bool Database::createPosition(string symName, string accountId, float amount){
    try{
        string check_postion_owner_valid_sql = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID = " + accountId + ";";
        pqxx::nontransaction N1(*C);
        pqxx::result R1( N1.exec(check_postion_owner_valid_sql));
        pqxx::result::const_iterator c1 = R1.begin();
        //if position's account does not exist, return false 
        if( c1 == R1.end()){
            cout << " position's account does not exist" << endl;
            return false;
        }
        N1.commit();
        string check_postion_exist_sql = "SELECT * FROM POSITIONS WHERE ACCOUNT_ID = " + accountId + " AND SYM = '" + symName + "';";
        pqxx::nontransaction N2(*C);
        pqxx::result R2( N2.exec(check_postion_exist_sql));      
        pqxx::result::const_iterator c2 = R2.begin();
        //if position does not exist, create a position, return true 
        if( c2 == R2.end()){
            cout << " in not exist "  << endl;
            string create_pos_sql = "INSERT INTO POSITIONS (ACCOUNT_ID, SYM, AMOUNT) "		\
	                                "VALUES (" + accountId + ", '" + symName + "', " + to_string(amount) + ");";
            N2.commit();
            pqxx::work W(*C);
            W.exec(create_pos_sql);           
            W.commit();
            cout << " create a position" << endl;
            return true;
        }
        //exist, then update and return true
        else{
            //cout << " CORIGINAL AMOUNT " << c2[0].as<int>() << endl;
            //int updateAmount = c2[0].as<int>() + amount;
            string update_pos_sql = "UPDATE POSITIONS set AMOUNT=AMOUNT+" + to_string(amount) + " WHERE ACCOUNT_ID=" + accountId + " AND SYM='" + symName + "';";
            N2.commit();
            pqxx::work W(*C);
            
            W.exec(update_pos_sql);
            W.commit();
            cout << " update a position" << endl;
            return true;
        }
    }
    catch (const std::exception &e) {
        cerr << "errot:" <<e.what() << std::endl;
    }
    return false;
}

bool Database::createOrder(string symName, string accountId, float amount, float limit){
    try{
        time_t currTime = time(NULL);
        string create_order_sql ="INSERT INTO ORDER_OPEN (ACCOUNT_ID, SYM, AMOUNT, LIMITS, TIME) VALUES (" +\
                                accountId + ", '" + symName + "', " + to_string(amount) + ", " +\
                                to_string(limit) + ", " + to_string(currTime) + ");";
            pqxx::work W(*C);
            W.exec(create_order_sql);
            W.commit();
            cout << " create an order" << endl;
            return true;
    }
    catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
    return false;
}


bool Database::cancel(string accountId, string transId, vector<CancelOrder> &cancelOpenSet, vector<ExecutedOrder> &cancelExecutedSet){
    try{
        //cancel an open order in ORDER_OPEN table
        string open_order_find_sql = "SELECT AMOUNT from ORDER_OPEN WHERE ACCOUNT_ID =" + accountId + "AND id=" + transId + ";";
        pqxx::nontransaction N1(*C);
        pqxx::result R1( N1.exec(open_order_find_sql));  
        N1.commit();
        for(pqxx::result::const_iterator c1 = R1.begin(); c1 != R1.end(); ++c1){
            time_t currTime = time(NULL);
            float cancelAmount = c1[0].as<float>();
            cancelOpenSet.push_back(CancelOrder(cancelAmount, currTime));
            string add_cancel_from_open_order_sql ="INSERT INTO ORDER_CANCEL (ACCOUNT_ID, TRANS_ID, AMOUNT, TIME) VALUES (" +\
                                accountId + ", " + transId + ", " + to_string(cancelAmount) + ", " + to_string(currTime) + ");";
            string delete_cancel_from_open_order_sql = "DELETE FROM ORDER_OPEN WHERE id=" + transId + ";";
            pqxx::work W(*C);
            W.exec(add_cancel_from_open_order_sql);
            W.exec(delete_cancel_from_open_order_sql);
            W.commit();
            cout << " cancel an open order" << endl;
            //return true;
        }
        //search in execution order and find same trans id executed order
        string execute_order_find_sql = "SELECT PRICE, AMOUNT, TIME from ORDER_EXECUTION WHERE (BUYER_ID = " + accountId + " OR SELLER_ID = " + accountId + ")" 
                                                                                            + "AND (BUY_ID = " + transId + " OR SELL_ID = " + transId + ");";
        pqxx::nontransaction N2(*C);
        pqxx::result R2( N2.exec(execute_order_find_sql));  
        N2.commit();
        for(pqxx::result::const_iterator c2 = R2.begin(); c2 != R2.end(); ++c2){
            float cancelExcutedPrice= c2[0].as<float>();
            float cancelExecutedShare = c2[0].as<float>();
            time_t cancelExecutedTime = c2[0].as<time_t>();
            cancelExecutedSet.push_back(ExecutedOrder(cancelExcutedPrice, cancelExecutedShare, cancelExecutedTime));
            cout << "find a execute partial order when cancel it" << endl;
        }
        return true;   
    }
    catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
    return false;
}

bool Database::query(string accountId, string transId, vector<OpenOrder> &queryOpenSet, vector<CancelOrder> &queryCancelSet, vector<ExecutedOrder> &queryExecutedSet){
    try{
        string query_open_order_find_sql = "SELECT AMOUNT from ORDER_OPEN WHERE ACCOUNT_ID = " + accountId + " AND id = " + transId + ";" ;
        pqxx::nontransaction N1(*C);
        pqxx::result R1( N1.exec(query_open_order_find_sql));  
        N1.commit();
        for(pqxx::result::const_iterator c = R1.begin(); c != R1.end(); ++c){
            float queryOpenAmount= c[0].as<float>();
            queryOpenSet.push_back(OpenOrder(queryOpenAmount));
            cout << "find an open order when query" << endl;
        }
        string query_cancel_order_find_sql = "SELECT AMOUNT, TIME from ORDER_CANCEL WHERE ACCOUNT_ID = " + accountId + " AND id = " + transId + ";" ;
        pqxx::nontransaction N2(*C);
        pqxx::result R2( N2.exec(query_cancel_order_find_sql));  
        N2.commit();
        for(pqxx::result::const_iterator c = R2.begin(); c != R2.end(); ++c){
            float queryCancelAmount= c[0].as<float>();
            time_t queryCancelTime = c[0].as<time_t>();
            queryCancelSet.push_back(CancelOrder(queryCancelAmount, queryCancelTime));
            cout << "find a cancel order when query" << endl;
        }                                                                                      
        string query_execute_order_find_sql = "SELECT PRICE, AMOUNT, TIME from ORDER_EXECUTION WHERE (BUYER_ID = " + accountId + " OR SELLER_ID = " + accountId + ")"
                                                                                                + "AND (BUY_ID = " + transId + " OR SELL_ID = " + transId + ");";
        pqxx::nontransaction N3(*C);
        pqxx::result R3( N3.exec(query_execute_order_find_sql));  
        N3.commit();
        for(pqxx::result::const_iterator c = R3.begin(); c != R3.end(); ++c){
            float queryExcutePrice= c[0].as<float>();
            float queryExcuteShare = c[0].as<float>();
            time_t queryExcuteTime = c[0].as<time_t>();
            queryExecutedSet.push_back(ExecutedOrder(queryExcutePrice, queryExcuteShare, queryExcuteTime));
            cout << "find a execute order when query" << endl;
        } 
        return true;                                                                                    
    }
    catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
    return false;
}


bool Database::checkAccountValid(string accountId){
    try{
        string check_account_sql = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID = " + accountId + ";";
        pqxx::nontransaction N(*C);    
        pqxx::result R( N.exec(check_account_sql));
        N.commit();
        pqxx::result::const_iterator c = R.begin(); 
        if( c == R.end()){     
            return false;
        }
    }catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
    return true;
}

bool Database::checkBalance(string accountId, float amount, float limits){

}

bool Database::checkAmount(string accountId, float amount, string symName){

}


