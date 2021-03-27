#ifndef PARSER_H
#define PARSER_H
#include "common.h"
#include "pugixml/src/pugixml.hpp"

String do_create(pugi::xml_document& doc);
String do_transactions(pugi::xml_document& doc);
#endif