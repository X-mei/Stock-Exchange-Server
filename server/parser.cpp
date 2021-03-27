#include "parser.h"

string do_create(pugi::xml_document& doc, database& db){
    // xml_doc to store response
    pugi::xml_document result;
    pugi::xml_node outer = doc.append_child("result");
    // iterate through each child of create
    for (pugi::xml_node n: doc.child("create")){
        // to create account
        if (n.name() == "account"){
            string id = n.attribute("id").value();
            float balance = n.attribute("balance").as_float();
            // check if create account is successful
            if(db.createAccount(id, balance)){
                pugi::xml_node ac = outer.append_child("created");
                ac.append_attribute("id") = id;
            }
            // form error message if not successful
            else {
                pugi::xml_node ac = outer.append_child("error");
                ac.append_attribute("id") = id;
                ac.append_child(pugi::node_pcdata).set_value("Failed to create account");
            }
        }
        // to create position
        else if (n.name() == "symbol"){
            string sym = n.attribute("sym").value();
            // iterate through all position create request
            for (pugi::xml_node sub_n: n.children()){
                // create each response
                string id = sub_n.attribute("id").value();
                float num = sub_n.text().as_float();
                if (db.createPosition(sym, id, num)){
                    pugi::xml_node po = outer.append_child("created");
                    po.append_attribute("sym") = sym;
                    po.append_attribute("id") = id;
                }
                else {
                    pugi::xml_node po = outer.append_child("error");
                    po.append_attribute("sym") = sym;
                    po.append_attribute("id") = id;
                    po.append_child(pugi::node_pcdata).set_value("Failed to create position");
                }
            }
        }
        // invalid node name
        else {
            throw std::invalid_argument("XML not legal.");
        }
        // save xml into ss and return in the form of string
        stringstream ss;
        result.save(ss);
        return ss.str();
    }
    return ;
}

String do_transactions(pugi::xml_document& doc, database& db){
    // xml_doc to store response
    pugi::xml_document result;
    pugi::xml_node outer = doc.append_child("result");
    String account_id = doc.attribute("id").value();
    // if account do not exist, return xml with error message
    if (!db.checkAccountValid()){
        head.append_child("error").text().set("Invalid account ID");
        stringstream ss;
        result.save(ss);
        return ss.str();
    }
    // parse the xml, traverse all child of transactions
    for (pugi::xml_node node: doc.child("transactions")){
        if (node.name() == "order"){
            String sym = node.attribute("sym").value();
            float amount = node.attribute("amount").as_float();
            float price = node.attribute("limit").as_float();
            // create a temp error tag, change it to opened if no error occured
            pugi::xml_node error = outer.append_child("error");
            error.append_attribute("sym") = sym;
            error.append_attribute("amount") = amount;
            error.append_attribute("limit") = price;
            // if its a buy order
            if (amount > 0){
                if (!db.checkBalance(account_id, amount, price)){
                    error.text().set_value("Unable to buy: insefficient funds");
                    continue;
                }
            }
            // if its a sell order
            else if (amount <0){
                if (!db.checkAmount(account_id, sym, amount)){
                    error.text().set_value("Unable to sell: insefficient shares");
                    continue;
                }
            }
            // if amount is 0
            else {
                error.text().set_value("Amount cannot be zero");
                continue;
            }
            // create order, get correspond order id
            int order_id = createOrder(sym, account_id, amount, price);
            if (order_id == -1){
                error.text().set_value("Amount cannot be zero");
                continue;
            }
            // 
            if (!db.executeOrder(account_id, order_id, sym, amount, price)){
                error.text().set_value("Error executing order");
                continue;
            }
            else {
                error.set_name("opened");
            }
        }
        // handle query request
        else if (node.name() == "query"){
            // create parent node with acquired transaction id
            int transaction_id = node.attribute("id").as_int();
            pugi::xml_node status = outer.append_child("status");
            status.append_attribute("id") = transaction_id;
            // vectors to store query result
            vector<OpenOrder> Open;
            vector<CancelOrder> notExcecuted;
            vector<ExecutedOrder> Executed;
            // do query on database and fill up these vectors, traverse them to build result xml
            if (db.query(account_id, transaction_id, Open, notExcecuted, Executed)){
                for (OpenOrder opn: Open){
                    status.append_child("open").append_attribute("shares").set_value(opn.shares);
                }
                for (CancelOrder can: notExcecuted){ 
                    pugi::xml_node notEx = status.append_child("canceled");
                    notEx.append_attribute("shares") = can.shares;
                    notEx.append_attribute("time") = can.time;
                }
                for (ExecutedOrder exe: Executed){
                    pugi::xml_node Ex = status.append_child("executed");
                    Ex.append_attribute("shares") = exe.shares;
                    Ex.append_attribute("price") = exe.prices;
                    Ex.append_attribute("time") = exe.time;
                }
            }
            // create error response when given transaction id can't be found
            else {
                status.append_child("error").text().set("Transaction ID not found");
            }
        }
        // handle cancel request
        else if (node.name() == "cancel"){
            // create parent node with acquired transaction id
            int transaction_id = node.attribute("id").as_int();
            pugi::xml_node canceled = outer.append_child("canceled");
            canceled.append_attribute("id") = transaction_id;
            // vectors to store query result
            vector<CancelOrder> notExcecuted;
            vector<ExecutedOrder> Executed;
            // do query on database and fill up these vectors, traverse them to build result xml
            if (db.cancel(account_id, transaction_id, notExcecuted, Executed)){
                for (CancelOrder can: notExcecuted){
                    pugi::xml_node notEx = canceled.append_child("canceled");
                    notEx.append_attribute("shares") = can.shares;
                    notEx.append_attribute("time") = can.time;//change if not compatible
                }
                for (ExecutedOrder exe: Executed){
                    pugi::xml_node Ex = canceled.append_child("executed");
                    Ex.append_attribute("shares") = exe.shares;
                    Ex.append_attribute("price") = exe.prices;
                    Ex.append_attribute("time") = exe.time;//change if not compatible
                }
            }
            // create error response when given transaction id can't be found
            else {
                canceled.append_child("error").text().set("Transaction ID not found");
            }
        }
        // throw exception when the top-level tag is not valid
        else {
            throw std::invalid_argument("XML not legal.");
        }
    }
    // save xml into ss and return in the form of string
    stringstream ss;
    result.save(ss);
    return ss.str();
}