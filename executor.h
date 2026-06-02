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
    Database* db;

    // Evaluate full expression tree (AND / OR / leaf)
    bool evaluateExpression(Row row, ExpressionNode* node);

    // Evaluate single condition
    bool evaluateLeafCondition(Row row, ExpressionNode* node);

public:
    Executor(Database* database);

    // Executes query and prints result
    void execute(QueryNode* query);
};

#endif