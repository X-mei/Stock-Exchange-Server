#ifndef PARSER_H
#define PARSER_H
#include "common.h"
#include "database.h"
static mutex mtx;
string do_create(pugi::xml_document& doc, Database* db);
string do_transactions(pugi::xml_document& doc, Database* db);
#endif