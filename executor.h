#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "database.h"
#include "ast.h"

class Executor
{
private:

    Database* db;

    bool evaluateLeafCondition(
        Row row,
        ExpressionNode* node);

    bool evaluateExpression(
        Row row,
        ExpressionNode* node);

public:

    Executor(Database* database);

    void execute(QueryNode* query);
};

#endif