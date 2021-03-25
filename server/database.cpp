

void Database::create_tables() {
    string account_sql = "CREATE TABLE ACCOUNT("\
    "id     INT        AUTO_INCREMENT  PRIMARY KEY   NOT NULL,"			\
    "BALANCE        INT        NOT NULL);";
  
    string position_sql =
    "CREATE TABLE POSITIONS("			\
    "id INT AUTO_INCREMENT PRIMARY KEY NOT NULL,"				\
    "ACCOUNT_ID INT NOT NULL,"\
    "FOREIGN KEY(ACCOUNT_ID) REFERENCES ACCOUNT(id) ," \
    "SYM            VARCHAR(50)     NOT NULL,"				\
    "AMOUNT         FLOAT         NOT NULL);";
  
    string order_sql =
    "CREATE TABLE ORDER_OPEN("			\
    "id    INT        AUTO_INCREMENT  PRIMARY KEY   NOT NULL,"				\
    "ACCOUNT_ID INT NOT NULL,"\
    "FOREIGN KEY(ACCOUNT_ID)     INT       REFERENCES ACCOUNT(id)  NOT NULL," \
    "SYM            VARCHAR(50)     NOT NULL,"				\
    "AMOUNT         FLOAT         NOT NULL,"				\
    "PRICE          FLOAT        NOT NULL,"				\
    "TIME           TIMESTAMP      DEFAULT NOW());";
  
    string execution_sql =			\
    "CREATE TABLE ORDER_EXECUTION("			\
    "id    INT        AUTO_INCREMENT  PRIMARY KEY   NOT NULL,"	\
    "FOREIGN KEY(BUYER_ID)     INT       REFERENCES ACCOUNT(id)  NOT NULL,"	\
    "FOREIGN KEY(SELLER_ID)     INT       REFERENCES ACCOUNT(id)  NOT NULL,"	\
    "FOREIGN KEY(BUY_ID)     INT       REFERENCES ORDER_OPEN(id)  NOT NULL,"	\
    "FOREIGN KEY(SELL_ID)     INT       REFERENCES ORDER_OPEN(id)  NOT NULL,"	\
    "SYM            VARCHAR(50)     NOT NULL,"	\
    "PRICE          FLOAT        NOT NULL,"	\
    "AMOUNT         FLOAT         NOT NULL,"	\
    "TIME           TIMESTAMP      DEFAULT NOW());";

  string cancel_sql =				\
    "CREATE TABLE ORDER_CANCEL("			\
    "id    INT        AUTO_INCREMENT  PRIMARY KEY   NOT NULL,"	\
    "FOREIGN KEY(ACCOUNT_ID)     INT       REFERENCES ACCOUNT(id)  NOT NULL,"	\
    "FOREIGN KEY(TRANS_ID)     INT       REFERENCES ORDER_OPEN(id)  NOT NULL,"	\
    "AMOUNT         FLOAT        NOT NULL,"	\
    "TIME           TIMESTAMP      DEFAULT NOW());";
}