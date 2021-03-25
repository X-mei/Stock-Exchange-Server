#include "database.h"
Database::Database(){
    try{
        C = new pqxx::connection("dbname=testdb user=postgres password=cohondob hostaddr=127.0.0.1 port=5432");
        //this->C = temp;
      //C = new pqxx::connection("dbname=testdb user=postgres password=cohondob, hostaddr=127.0.0.1 port=5432");
        if (C->is_open()) {
            cout << "Opened database successfully: " << C.dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
            return;
        }
    }
    catch(const exception &e){
        cerr << e.what() << endl;
        return;
    }
};
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
    string drop_table_sql = "DROP TABLE ACCOUNT, POSITIONS, ORDER_OPEN, ORDER_EXECUTION, ORDER_CANCEL;";
    pqxx::work W(*C);
    W.exec(drop_table_sql);
    W.commit();
    cout << "Drop all previous tables successfully" << endl;
}

void Database::createTables() {
    pqxx::work W(*C);

    std::string account_sql = "CREATE TABLE ACCOUNT("\
    "id INT AUTO_INCREMENT PRIMARY KEY NOT NULL,"\
    "BALANCE FLOAT NOT NULL);";
    W.exec(account_sql);
    //cout << "create ACCOUNT successfully" << endl;
  
    std::string position_sql =
    "CREATE TABLE POSITIONS("\
    "id INT AUTO_INCREMENT PRIMARY KEY NOT NULL,"\
    "ACCOUNT_ID INT NOT NULL,"\
    "FOREIGN KEY(ACCOUNT_ID) REFERENCES ACCOUNT(id) ,"\
    "SYM VARCHAR(50) NOT NULL,"\
    "AMOUNT FLOAT NOT NULL);";
    W.exec(position_sql);
    //cout << "create POSITIONS successfully" << endl;
  
    std::string order_sql =
    "CREATE TABLE ORDER_OPEN("\
    "id INT AUTO_INCREMENT PRIMARY KEY  NOT NULL,"\
    "ACCOUNT_ID INT NOT NULL,"\
    "FOREIGN KEY(ACCOUNT_ID) REFERENCES ACCOUNT(id),"\
    "SYM VARCHAR(50) NOT NULL,"\
    "AMOUNT FLOAT NOT NULL,"\
    "PRICE FLOAT NOT NULL,"\
    "TIME TIMESTAMP DEFAULT NOW());";
    W.exec(order_sql);
    //cout << "create ORDER_OPEN successfully" << endl;
  
    std::string execution_sql =
    "CREATE TABLE ORDER_EXECUTION("\
    "id INT AUTO_INCREMENT PRIMARY KEY NOT NULL,"\
    "BUYER_ID INT NOT NULL,"\
    "SELLER_ID INT NOT NULL,"\
    "BUY_ID INT NOT NULL,"\
    "SELL_ID INT NOT NULL,"\
    "FOREIGN KEY(BUYER_ID) REFERENCES ACCOUNT(id),"	\
    "FOREIGN KEY(SELLER_ID) REFERENCES ACCOUNT(id),"\
    "FOREIGN KEY(BUY_ID) REFERENCES ORDER_OPEN(id),"\
    "FOREIGN KEY(SELL_ID) REFERENCES ORDER_OPEN(id),"\
    "SYM VARCHAR(50) NOT NULL,"\
    "PRICE FLOAT NOT NULL,"	\
    "AMOUNT FLOAT NOT NULL,"\
    "TIME TIMESTAMP DEFAULT NOW());";
    W.exec(execution_sql);
    //cout << "create ORDER_EXECUTION successfully" << endl;

    std::string cancel_sql =
    "CREATE TABLE ORDER_CANCEL("\
    "id INT AUTO_INCREMENT PRIMARY KEY NOT NULL,"\
    "ACCOUNT_ID INT NOT NULL,"\
    "TRANS_ID INT NOT NULL,"\
    "FOREIGN KEY(ACCOUNT_ID) REFERENCES ACCOUNT(id),"	\
    "FOREIGN KEY(TRANS_ID) REFERENCES ORDER_OPEN(id),"	\
    "AMOUNT FLOAT NOT NULL,"\
    "TIME TIMESTAMP DEFAULT NOW());";
    W.exec(cancel_sql);
    //cout << "create ORDER_CANCEL successfully" << endl;

    W.commit();
    cout << "create all tables successfully" << endl;
}

bool Database::createAccount(string accountId, float balance){
    try{
        string select_account_sql = "SELECT id FROM ACCOUNT WHERE id = " + accountId + ";";
        pqxx::nontransaction N(*C);
        pqxx::result R( N.exec(select_account_sql));
        pqxx::result::const_iterator c = R.begin(); 
        if( c != R.end()){
            return false;
        }
        string create_new_accoun_sql = "INSERT INTO ACCOUNT (id, BALANCE) VALUES (" + accountId + ", " + to_string(balance) + ");";
        pqxx::work W(*C);
        W.exec(create_new_accoun_sql);
        W.commit();
    }
    catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
    return true;
}

bool Database::createPosition(string symName, string accountId, float amount){
    try{
        string check_postion_owner_valid_sql = "SELECT id FROM ACCOUNT WHERE id = " + accountId + ";";
        pqxx::nontransaction N1(*C);
        pqxx::result R1( N1.exec(check_postion_owner_valid_sql));
        pqxx::result::const_iterator c1 = R1.begin();
        //if position's account does not exist, return false 
        if( c1 == R1.end()){
            return false;
        }
        string check_postion_exist_sql = "SELECT SYM FROM POSITIONS WHERE SYM = " + symName + ";";
        pqxx::nontransaction N2(*C);
        pqxx::result R2( N2.exec(check_postion_owner_valid_sql));
        pqxx::result::const_iterator c2 = R2.begin();
        //if position does not exist, create a position, return true 
        if( c2 == R2.end()){
            string create_pos_sql = "INSERT INTO POSITION (ACCOUNT_ID, SYM, AMOUNT) "		\
	                                "VALUES (" + accountId + ", '" + symName + "', " + to_string(amount) + ");";
            pqxx::work W(*C);
            W.exec(create_pos_sql);
            W.commit();
            return true;
        }
        //exist, then update and return true
        else{
            int updateAmount = c2[0].as<int>() + amount;
            string update_pos_sql = "UPDATE POSITION set AMOUNT=" + to_string(updateAmount) + " WHERE ACCOUNT_ID=" + accountId + " AND SYM='" + symName + "';";
            pqxx::work W(*C);
            W.exec(update_pos_sql);
            W.commit();
            return true;
        }
    }
    catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
    return false;
}

bool Database::createOrder(string symName, string accountId, float amount, float price){
    try{
        time_t currTime = time(NULL);
        string create_order_sql ="INSERT INTO ORDER_OPEN (ACCOUNT_ID, SYM, AMOUNT, PRICE, TIME) VALUES (" +\
                                accountId + ", '" + symName + "', " + to_string(amount) + ", " +\
                                to_string(price) + ", " + to_string(currTime) + ");";
            pqxx::work W(*C);
            W.exec(create_order_sql);
            W.commit();
            return true;
    }
    catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
    return false;
}