#ifndef ANALYZER_H
#define ANALYZER_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "ast.h"
#include "database.h"

using namespace std;

class SemanticAnalyzer
{
private:
    Database* db;

    DataType getColumnType(string tableName, string columnName);

    bool tableExists(string tableName);

    bool columnExists(string tableName, string columnName);

    bool validateExpression(ExpressionNode *node, string tableName);

    bool isOperatorValid(DataType type, string op);

    DataType getLiteralType(string value);

public:
    string errorMessage;

    SemanticAnalyzer(Database* database);

    bool validate(QueryNode *query);
};

#endif