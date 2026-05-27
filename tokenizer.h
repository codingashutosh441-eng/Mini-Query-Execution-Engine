#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

struct Token
{
    string value;
    string type;
};

extern vector<Token> tokens;

extern set<string> mysqlKeywords;

extern set<string> ops;

// tokenizer functions
bool isNumber(string s);

bool isIdentifier(string s);

void tokenizer(string s);

#endif