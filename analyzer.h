#ifndef ANALYZER_H
#define ANALYZER_H

#include <iostream>
#include <map>
#include <vector>
#include <string>

#include "ast.h"

using namespace std;

class SemanticAnalyzer
{
private:

    map<string, vector<string>> schema;

    bool tableExists(string tableName);

    bool columnExists(
        string tableName,
        string columnName);

public:

    string errorMessage;

    SemanticAnalyzer();

    bool validate(QueryNode* query);
};

#endif