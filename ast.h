#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

enum class DataType
{
    INT,
    STRING,
    FLOAT
};

enum class AggregateType
{
    COUNT,
    SUM,
    AVG,
    MIN,
    MAX
};

struct GroupByNode
{
    vector<string> columns;
};

struct AggregateNode
{
    AggregateType type;

    string column;

    bool countStar = false;
};

struct ColumnDefinitionNode
{
    string name;
    DataType type;
};

struct CreateTableNode
{
    string tableName;
    vector<ColumnDefinitionNode> columns;
};

struct ColumnNode
{
    vector<string> columns;

    vector<AggregateNode> aggregates;

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

    GroupByNode* groupBy = nullptr;
};

struct InsertValueNode
{
    string value;
    DataType type;
};

struct InsertRowNode
{
    vector<InsertValueNode> values;
};

struct InsertNode
{
    string tableName;
    vector<InsertRowNode> rows;
};

struct UpdateNode
{
    string tableName;

    string columnName;
    string newValue;
    DataType valueType;

    ExpressionNode* whereExpression = nullptr;
};

struct DeleteNode
{
    string tableName;

    ExpressionNode* whereExpression = nullptr;
};

void printTree(QueryNode* root);
void freeQuery(QueryNode* root);

void printExpressionTree(ExpressionNode *node, int level);
void freeExpressionTree(ExpressionNode *node);
void freeDelete(DeleteNode* node);

#endif