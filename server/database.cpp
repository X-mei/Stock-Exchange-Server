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
            //cout << " in not exist "  << endl;
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

int Database::createOrder(string symName, string accountId, float amount, float limit){
    try{
        if(amount < 0){ //sell, check amount 
            checkAmount(accountId, amount, symName);
        }
        else{
            checkBalance(accountId, amount, limit);
        }
        time_t currTime = time(NULL);
        string create_order_sql ="INSERT INTO ORDER_OPEN (ACCOUNT_ID, SYM, AMOUNT, LIMITS, TIME) VALUES (" +\
                                accountId + ", '" + symName + "', " + to_string(amount) + ", " +\
                                to_string(limit) + ", " + to_string(currTime) + ");";
        pqxx::work W(*C);
        W.exec(create_order_sql);
        W.commit();
        pqxx::nontransaction N(*C);
        string get_trans_id_sql = "SELECT id FROM ORDER_OPEN "\
                                "WHERE TIME=" +\
                                to_string(currTime) + " AND ACCOUNT_ID = " + accountId + "AND SYM = '" + symName + \
                                "' AND AMOUNT = " + to_string(amount) + "AND LIMITS = " + to_string(limit) + ";";

        pqxx::result R( N.exec(get_trans_id_sql));
        N.commit();
        pqxx::result::const_iterator c = R.begin();
        int trans_id = c[0].as<int>();
        cout << " create open order No." << trans_id << endl;
        executeOrder(accountId, to_string(trans_id), symName, amount, limit, currTime);
        return trans_id;
    }
    catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
    return -1;
}

bool Database::executeOrder(string accountId, string transId, string symName, float amount, float limit, time_t orderTime){
    try{
        if(amount > 0){//for buy, amount > 0
            cout << "in execute buy order" << endl;
            string find_match_sell_sql = "SELECT id, ACCOUNT_ID, AMOUNT, LIMITS, TIME from ORDER_OPEN WHERE "\
                                    "AMOUNT<0 AND LIMITS<=" + to_string(limit) + " AND SYM='" + symName + "' AND ACCOUNT_ID!=" + accountId  + " ORDER BY LIMITS ASC, TIME ASC;";
            pqxx::nontransaction N1(*C);
            pqxx::result R1(N1.exec(find_match_sell_sql));
            N1.commit();
            float restBuyAmount = amount;
            //bool buyMarker = true;
            for(pqxx::result::const_iterator c = R1.begin(); c != R1.end(); ++c){
                //cout << "in for" << endl;
                if(restBuyAmount <= 0){ //this buy order is all processed
                    cout << "this buy order is all processed" << endl;
                    break;
                }
                string sellId = c[0].as<string>();
                string sellerAccountId = c[1].as<string>();
                float sellAmount = c[2].as<float>();
                float sellLimit = c[3].as<float>();
                time_t sellTime = c[4].as<time_t>();
                if(sellTime < orderTime){//use sell limit
                    if(restBuyAmount + sellAmount > 0){//buy > sell, partial buy, delete sell open order, update buy order
                        float total1 = (limit -sellLimit) * (-sellAmount);  //sell add gap balance, positive
                        processSingleTrade(accountId, sellerAccountId, transId, sellId, symName, -sellAmount, sellLimit, -sellAmount, -total1);//add partial buy in ORDER_EXECUTE
                        float total2 = sellLimit * sellAmount;  //negative
                        processSingleTrade(accountId, sellerAccountId, transId, sellId, symName, sellAmount, sellLimit, 0, total2);//add whole sell in ORDER_EXECUTE
                        deleteSingleOrder(sellerAccountId, sellId, symName, sellLimit, sellTime);   //delete sell
                        updateSingleOrder(accountId, transId, symName, sellAmount, limit, orderTime);   //update buy
                        //restBuyAmount = restBuyAmount + sellAmount;
                        //continue;
                    }
                    else if(restBuyAmount + sellAmount < 0){//buy < sell, partial sell, end this buy process
                        float total1 = (limit -sellLimit) * (restBuyAmount);  //sell add gap balance, positive
                        processSingleTrade(accountId, sellerAccountId, transId, sellId, symName, restBuyAmount, sellLimit, restBuyAmount, -total1);//add whole buy in ORDER_EXECUTE
                        float total2 = sellLimit * (-restBuyAmount);  //negative
                        processSingleTrade(accountId, sellerAccountId, transId, sellId, symName, -restBuyAmount, sellLimit, 0, total2);//add partial sell in ORDER_EXECUTE
                        deleteSingleOrder(accountId, transId, symName, limit, orderTime);  //delete buy 
                        updateSingleOrder(sellerAccountId, sellId, symName, restBuyAmount, sellLimit, sellTime);   //update sell
                        //continue;
                    }
                    else{//buy amount = sell amount, perfect match
                        float total1 = (limit -sellLimit) * (restBuyAmount);  //sell add gap balance, positive
                        processSingleTrade(accountId, sellerAccountId, transId, sellId, symName, restBuyAmount, sellLimit, restBuyAmount, -total1);//add whole buy in ORDER_EXECUTE
                        float total2 = sellLimit * sellAmount;  //negative
                        processSingleTrade(accountId, sellerAccountId, transId, sellId, symName, sellAmount, sellLimit, 0, total2);//add whole sell in ORDER_EXECUTE
                        deleteSingleOrder(accountId, transId, symName, limit, orderTime);  //delete buy
                        deleteSingleOrder(sellerAccountId, sellId, symName, sellLimit, sellTime);   //delete sell
                    }
                }
                else{//use buy limit
                    if(restBuyAmount + sellAmount > 0){//buy > sell, partial buy
                        //float total1 = (limit -sellLimit) * (restBuyAmount);  //sell add gap balance, positive
                        float total1 = limit * sellAmount;  //negative
                        processSingleTrade(accountId, sellerAccountId, transId, sellId, symName, -sellAmount, limit, -sellAmount, 0);//add partial buy in ORDER_EXECUTE
                        processSingleTrade(accountId, sellerAccountId, transId, sellId, symName, sellAmount, limit, 0, total1);//add whole sell in ORDER_EXECUTE
                        deleteSingleOrder(sellerAccountId, sellId, symName, sellLimit, sellTime);   //delete sell
                        updateSingleOrder(accountId, transId, symName, sellAmount, limit, orderTime);   //update buy
                        //restBuyAmount += sellAmount;
                        //continue;
                    }
                    else if(restBuyAmount + sellAmount > 0){//buy < sell, partial sell, end this buy process
                        float total1 = limit * (-restBuyAmount);  //negative
                        processSingleTrade(accountId, sellerAccountId, transId, sellId, symName, restBuyAmount, limit, restBuyAmount, 0);//add whole buy in ORDER_EXECUTE
                        processSingleTrade(accountId, sellerAccountId, transId, sellId, symName, -restBuyAmount, limit, 0, total1);//add partial sell in ORDER_EXECUTE
                        deleteSingleOrder(accountId, transId, symName, limit, orderTime);  //delete buy 
                        updateSingleOrder(sellerAccountId, sellId, symName, restBuyAmount, sellLimit, sellTime);   //update sell
                        //continue;
                    }
                    else{//buy amount = sell amount, perfect match
                        float total1 = limit * sellAmount;  //negative
                        processSingleTrade(accountId, sellerAccountId, transId, sellId, symName, restBuyAmount, limit, restBuyAmount, 0);//add whole buy in ORDER_EXECUTE
                        processSingleTrade(accountId, sellerAccountId, transId, sellId, symName, sellAmount, limit, 0, total1);//add whole sell in ORDER_EXECUTE
                        deleteSingleOrder(accountId, transId, symName, limit, orderTime);  //delete buy
                        deleteSingleOrder(sellerAccountId, sellId, symName, sellLimit, sellTime);   //delete sell
                    }
                }
                restBuyAmount = restBuyAmount + sellAmount; //update rest of buy amount
            }
        }
        else if(amount < 0){//for sell, amount > 0
            cout << "in execute sell order" << endl;
            string find_match_buy_sql = "SELECT id, ACCOUNT_ID, AMOUNT, LIMITS, TIME from ORDER_OPEN WHERE "\
                                    "AMOUNT>0 AND LIMITS>=" + to_string(limit) + " AND SYM='" + symName + "' AND ACCOUNT_ID!=" + accountId  + " ORDER BY LIMITS DESC, TIME ASC;";
            pqxx::nontransaction N1(*C);
            pqxx::result R1(N1.exec(find_match_buy_sql));
            N1.commit();
            float restSellAmount = amount;
            //bool buyMarker = true;
            for(pqxx::result::const_iterator c = R1.begin(); c != R1.end(); ++c){
                cout << "in for" << endl;
                if(restSellAmount >= 0){ //this buy order is all processed
                    cout << "this buy order is all processed" << endl;
                    break;
                }
                string buyId = c[0].as<string>();
                cout << "buyId: " << buyId << endl;
                string buyerAccountId = c[1].as<string>();
                float buyAmount = c[2].as<float>();
                float buyLimit = c[3].as<float>();
                time_t buyTime = c[4].as<time_t>();
                if(buyTime < orderTime){//use buy limit
                    if(restSellAmount + buyAmount > 0){//buy > sell, partial buy, delete sell open order, update buy order
                        float total1 = buyLimit * restSellAmount;  //negative
                        processSingleTrade(buyerAccountId, accountId, buyId, transId, symName, -restSellAmount, buyLimit, -restSellAmount, 0);//add partial buy in ORDER_EXECUTE
                        processSingleTrade(buyerAccountId, accountId, buyId, transId, symName, restSellAmount, buyLimit, 0, total1);//add whole sell in ORDER_EXECUTE
                        deleteSingleOrder(accountId, transId, symName, limit, orderTime);   //delete sell
                        updateSingleOrder(buyerAccountId, buyId, symName, restSellAmount, buyLimit, buyTime);   //update buy
                        //restbuyAmount = restbuyAmount + buyAmount;
                        //continue;
                    }
                    else if(restSellAmount + buyAmount < 0){//buy < sell, partial sell, delete buy open order, update sell order
                        float total1 = buyLimit * (-buyAmount);  //negative
                        processSingleTrade(buyerAccountId, accountId, buyId, transId, symName, buyAmount, buyLimit, buyAmount, 0);//add whole buy in ORDER_EXECUTE
                        processSingleTrade(buyerAccountId, accountId, buyId, transId, symName, -buyAmount, buyLimit, 0, total1);//add partial sell in ORDER_EXECUTE
                        deleteSingleOrder(buyerAccountId, buyId, symName, buyLimit, buyTime);  //delete buy 
                        updateSingleOrder(accountId, transId, symName, buyAmount, limit, orderTime);   //update sell
                        //continue;
                    }
                    else{//buy amount = sell amount, perfect match
                        float total1 = buyLimit * (-buyAmount);  //negative
                        processSingleTrade(buyerAccountId, accountId, buyId, transId, symName, buyAmount, buyLimit, buyAmount, 0);//add whole buy in ORDER_EXECUTE
                        processSingleTrade(buyerAccountId, accountId, buyId, transId, symName, restSellAmount, buyLimit, 0, total1);//add whole sell in ORDER_EXECUTE
                        deleteSingleOrder(accountId, transId, symName, limit, orderTime);  //delete sell
                        deleteSingleOrder(buyerAccountId, buyId, symName, buyLimit, buyTime);   //delete buy
                    }
                }
                else{//use sell limit
                    if(restSellAmount + buyAmount > 0){//buy > sell, partial buy, delete sell open order, update buy order
                        float total1 = (buyLimit -limit) * (-restSellAmount);  //sell add gap balance, positive
                        float total2 = limit * restSellAmount;  //negative
                        processSingleTrade(buyerAccountId, accountId, buyId, transId, symName, -restSellAmount, limit, -restSellAmount, -total1);//add partial buy in ORDER_EXECUTE
                        processSingleTrade(buyerAccountId, accountId, buyId, transId, symName, restSellAmount, limit, 0, total2);//add whole sell in ORDER_EXECUTE
                        deleteSingleOrder(accountId, transId, symName, limit, orderTime);   //delete sell
                        updateSingleOrder(buyerAccountId, buyId, symName, restSellAmount, buyLimit, buyTime);   //update buy
                        //restbuyAmount = restbuyAmount + buyAmount;
                        //continue;
                    }
                    else if(restSellAmount + buyAmount < 0){//buy < sell, partial sell, delete buy open order, update sell order
                        float total1 = (buyLimit -limit) * (buyAmount);  //sell add gap balance, positive
                        float total2 = limit * (-buyAmount);  //negative
                        processSingleTrade(buyerAccountId, accountId, buyId, transId, symName, buyAmount, limit, buyAmount, -total1);//add whole buy in ORDER_EXECUTE
                        processSingleTrade(buyerAccountId, accountId, buyId, transId, symName, -buyAmount, limit, 0, total2);//add partial sell in ORDER_EXECUTE
                        deleteSingleOrder(buyerAccountId, buyId, symName, buyLimit, buyTime);  //delete buy 
                        updateSingleOrder(accountId, transId, symName, buyAmount, limit, orderTime);   //update sell
                        //continue;
                    }
                    else{//buy amount = sell amount, perfect match
                        float total1 = (buyLimit -limit) * (buyAmount);  //sell add gap balance, positive
                        float total2 = limit * restSellAmount;  //negative
                        processSingleTrade(buyerAccountId, accountId, buyId, transId, symName, buyAmount, limit, buyAmount, -total1);//add whole buy in ORDER_EXECUTE
                        processSingleTrade(buyerAccountId, accountId, buyId, transId, symName, restSellAmount, limit, 0, total2);//add whole sell in ORDER_EXECUTE
                        deleteSingleOrder(accountId, transId, symName, limit, orderTime);  //delete sell
                        deleteSingleOrder(buyerAccountId, buyId, symName, buyLimit, buyTime);   //delete buy
                    }
                }
                restSellAmount = restSellAmount + buyAmount; //update rest of buy amount
            }
        }
        
    }
    catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

void Database::updateSingleOrder(string accountId, string transId, string symName, float amount, float limit, time_t orderTime){
    cout <<"in update an order" << endl;
    try{
        string update_single_order_sql = "UPDATE ORDER_OPEN set AMOUNT=AMOUNT+(" + to_string(amount) + \
                                ") WHERE ACCOUNT_ID=" + accountId + " AND SYM='" + symName +"' AND id = " \
                                + transId + "AND LIMITS = " + to_string(limit) + "AND TIME = " + to_string(orderTime) + ";";
        pqxx::work W(*C);
        W.exec(update_single_order_sql);
        W.commit();
    }
    catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

void Database::deleteSingleOrder(string accountId, string transId, string symName, float limit, time_t orderTime){
    cout <<"in delete an order while it is fully executed" << endl;
    try{
        string update_pos_sql = "DELETE FROM ORDER_OPEN WHERE ACCOUNT_ID=" + accountId + " AND SYM='" + symName +"' AND id = " \
                                + transId + "AND LIMITS = " + to_string(limit) + "AND TIME = " + to_string(orderTime) + ";";
        pqxx::work W(*C);
        W.exec(update_pos_sql);
        W.commit();
    }
    catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}

//process two matched order, add in to execution table
void Database::processSingleTrade(string buyAccountId, string sellAccountId, string buyTransId, string sellTransId, string symName, float amount, float price, float processAmount, float processTotal){
    cout << "int process single trade" << endl;
    try{
        time_t currTime = time(NULL);
        string creat_execution_order_sql = "INSERT INTO ORDER_EXECUTION (BUYER_ID, SELLER_ID, BUY_ID, SELL_ID, SYM, PRICE, AMOUNT, TIME) VALUES (" +\
                                            buyAccountId + ", " + sellAccountId + \
                                            ", " + buyTransId + ", " + sellTransId + \
                                            ", '" + symName + "', " + to_string(price) +		\
                                            ", " + to_string(amount) + ", " + to_string(currTime) + ");";
        pqxx::work W1(*C);
        W1.exec(creat_execution_order_sql);
        W1.commit();
        cout << "total = " << amount * price<<endl;
        if(amount <= 0){// sell order
            cout << "in update positions sell" << endl;
            string seller_update_position_sql = "UPDATE POSITIONS set AMOUNT=AMOUNT+(" + to_string(processAmount) + \
                                ") WHERE ACCOUNT_ID=" + sellAccountId + " AND SYM='" + symName +"';";
            string seller_update_account_sql = "UPDATE ACCOUNT set BALANCE=BALANCE-(" + to_string(processTotal) + \
                                ") WHERE ACCOUNT_ID=" + sellAccountId + ";";
            pqxx::work W2(*C);
            W2.exec(seller_update_position_sql);
            W2.commit();
            //W1.exec(seller_update_position_sql);
            //W1.exec(seller_update_account_sql); 
            pqxx::work W3(*C);
            W3.exec(seller_update_account_sql);
            W3.commit();
            return;                    
        }
        else{//buy order
            cout << "in update positions buy" << endl;
            string buyer_update_position_sql = "UPDATE POSITIONS set AMOUNT=AMOUNT+(" + to_string(processAmount) + \
                                ") WHERE ACCOUNT_ID=" + buyAccountId + " AND SYM='" + symName +"';";
            string buyer_update_account_sql = "UPDATE ACCOUNT set BALANCE=BALANCE-(" + to_string(processTotal) + \
                                ") WHERE ACCOUNT_ID=" + buyAccountId + ";";
            //W1.exec(buyer_update_position_sql);
            //W1.exec(buyer_update_account_sql);
            //W1.commit();
            pqxx::work W2(*C);
            W2.exec(buyer_update_position_sql);
            W2.commit();
            //W1.exec(seller_update_position_sql);
            //W1.exec(seller_update_account_sql); 
            pqxx::work W3(*C);
            W3.exec(buyer_update_account_sql);
            W3.commit(); 
            return;
        }
    }
    catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
}


bool Database::cancel(string accountId, string transId, vector<CancelOrder> &cancelOpenSet, vector<ExecutedOrder> &cancelExecutedSet){
    try{
        //cancel an open order in ORDER_OPEN table
        string open_order_find_sql = "SELECT AMOUNT, LIMITS, SYM from ORDER_OPEN WHERE ACCOUNT_ID =" + accountId + "AND id=" + transId + ";";
        pqxx::nontransaction N1(*C);
        pqxx::result R1( N1.exec(open_order_find_sql));  
        N1.commit();
        for(pqxx::result::const_iterator c1 = R1.begin(); c1 != R1.end(); ++c1){
            time_t currTime = time(NULL);
            float cancelAmount = c1[0].as<float>();
            float cancelLimit = c1[1].as<float>();
            string cancelSymName = c1[2].as<string>();
            if(cancelAmount < 0){//cancel sell, return amount
                string give_back_positions_amount_sql = "UPDATE POSITIONS set AMOUNT=AMOUNT-(" + to_string(cancelAmount) + \
                                ") WHERE ACCOUNT_ID=" + accountId + " AND SYM='" + cancelSymName +"';";
                pqxx::work W1(*C);
                W1.exec(give_back_positions_amount_sql);
                W1.commit();
            }
            else{//cancel buy, return balance
                float totalCancel = cancelAmount * cancelLimit;
                string give_back_account_balance_sql = "UPDATE ACCOUNT set BALANCE=BALANCE+(" + to_string(totalCancel) + \
                                ") WHERE ACCOUNT_ID=" + accountId + " ;";
                pqxx::work W1(*C);
                W1.exec(give_back_account_balance_sql);
                W1.commit();
                
            }
            
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
    try{
        string check_balance_sql = "SELECT BALANCE FROM ACCOUNT WHERE ACCOUNT_ID = " + accountId + ";";
        pqxx::nontransaction N(*C);    
        pqxx::result R( N.exec(check_balance_sql));
        N.commit();
        pqxx::result::const_iterator c = R.begin(); 
        if( c == R.end()){     
            return false;
        }
        float currentBalance= c[0].as<float>();
        if(currentBalance >= amount * limits){
            string update_balance_sql = "UPDATE ACCOUNT set BALANCE=BALANCE-(" + to_string(amount*limits) + \
                                ") WHERE ACCOUNT_ID=" + accountId + ";";
            pqxx::work W1(*C);
            W1.exec(update_balance_sql);
            W1.commit();
            return true;
        }
    }catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
    return false;
}

//only for sell
bool Database::checkAmount(string accountId, float amount, string symName){
        try{
        string check_amount_sql = "SELECT AMOUNT FROM POSITIONS WHERE ACCOUNT_ID = " + accountId + \
                                    " AND SYM='" + symName +"';";
        pqxx::nontransaction N(*C);    
        pqxx::result R( N.exec(check_amount_sql));
        N.commit();
        pqxx::result::const_iterator c = R.begin(); 
        if( c == R.end()){     
            return false;
        }
        float currentAmount= c[0].as<float>();
        if(currentAmount + amount >= 0){
            string update_amount_sql = "UPDATE POSITIONS set AMOUNT=AMOUNT+(" + to_string(amount) + \
                                ") WHERE ACCOUNT_ID=" + accountId + " AND SYM='" + symName +"';";
            pqxx::work W1(*C);
            W1.exec(update_amount_sql);
            W1.commit();
            return true;
        }
    }catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
    return false;
}


