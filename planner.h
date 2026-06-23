#ifndef PLANNER_H
#define PLANNER_H

#include <iostream>
#include <vector>
#include <string>

#include "ast.h"
#include "database.h"

using namespace std;

enum class StepType
{
    SCAN, INDEX_SEEK, FILTER, SORT,
    LIMIT, PROJECT, AGGREGATE, GROUP_BY, DISTINCT
};

struct ExecutionStep
{
    StepType type;
    string details;
};

class Planner
{
private:
    Database* db;

    vector<ExecutionStep> steps;

    string expressionToString(
        ExpressionNode* node);

public:
    Planner(Database* database);

    void createPlan(
        QueryNode* query);

    void printPlan();
};

#endif