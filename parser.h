#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

#include "tokenizer.h"
#include "ast.h"

extern string errorMessage;

extern int pos;

extern QueryNode *root;

// parser functions
bool expect(string val);

QueryNode* parseColumns();

bool parseWhere();

bool parseSelect();

#endif