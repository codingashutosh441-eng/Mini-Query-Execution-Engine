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
    struct ColumnInfo
    {
        string name;
        DataType type;
    };

    map<string, vector<ColumnInfo>> schema;

    DataType getColumnType(
    string tableName,
    string columnName);

    bool tableExists(string tableName);

    bool columnExists(
        string tableName,
        string columnName);

    bool validateExpression(
        ExpressionNode *node,
        string tableName);

    bool isOperatorValid(
    DataType type,
    string op);

public:
    string errorMessage;

    SemanticAnalyzer();

    bool validate(QueryNode *query);
};

#endif