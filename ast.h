#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

enum class DataType
{
    INT,
    STRING
};

struct ColumnNode
{
    vector<string> columns;
    bool selectAll = false;
};

struct TableNode
{
    string tableName;
};

struct OrderByNode
{
    string column;
    string direction;
};

struct LimitNode
{
    int count;
};

struct ExpressionNode
{
    bool isLogical = false;
    string logicalOp;

    string column;
    string op;
    string value;

    DataType valueType;

    ExpressionNode* left = nullptr;
    ExpressionNode* right = nullptr;
};

struct QueryNode
{
    ColumnNode* columns = nullptr;
    TableNode* table = nullptr;
    ExpressionNode* whereExpression = nullptr;

    OrderByNode* orderBy = nullptr;
    LimitNode* limit = nullptr;
};

void printTree(QueryNode* root);
void freeQuery(QueryNode* root);

void printExpressionTree(ExpressionNode *node, int level);
void freeExpressionTree(ExpressionNode *node);

#endif