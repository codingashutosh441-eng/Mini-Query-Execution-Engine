#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct ColumnNode
{
    vector<string> columns;

    bool selectAll = false;
};

struct TableNode
{
    string tableName;
};

struct ConditionNode
{
    string left;
    string op;
    string right;
};

struct QueryNode
{
    ColumnNode* columns = nullptr;

    TableNode* table = nullptr;

    ConditionNode* condition = nullptr;
};

void printTree(QueryNode* root);
void freeQuery(QueryNode* root);

#endif