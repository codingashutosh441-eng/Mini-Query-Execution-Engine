#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "database.h"
#include "ast.h"
#include <vector>
#include <string>

using namespace std;

class Executor
{
private:
    Database *db;

    // Evaluate full expression tree (AND / OR / leaf)
    bool evaluateExpression(
    const Row& row,
    const string& tableName,
    ExpressionNode* node);

bool evaluateLeafCondition(
    const Row& row,
    const string& tableName,
    ExpressionNode* node);
    string getCellValue(
        const Row &row,
        const string &tableName,
        const string &columnName);

    int getCellInt(
        const Row &row,
        const string &tableName,
        const string &columnName);

    Cell getCell(
    const Row& row,
    const string& tableName,
    const string& columnName);

public:
    Executor(Database *database);

    // Executes query and prints result
    void execute(QueryNode *query);
    void executeInsert(InsertNode *node);
    void executeUpdate(UpdateNode* node);
};

#endif