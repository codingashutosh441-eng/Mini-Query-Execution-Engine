#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "database.h"
#include "ast.h"

class Executor
{
private:
    Database* db;

    bool evaluateCondition(Row row,
                           ConditionNode* condition);

public:
    Executor(Database* database);

    void execute(QueryNode* query);
};

#endif