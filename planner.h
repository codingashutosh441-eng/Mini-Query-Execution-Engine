#ifndef PLANNER_H
#define PLANNER_H

#include <iostream>
#include <vector>
#include <string>

#include "ast.h"

using namespace std;

enum class StepType
{
    SCAN,
    FILTER,
    SORT,
    LIMIT,
    PROJECT,
    AGGREGATE,
    GROUP_BY
};

struct ExecutionStep
{
    StepType type;
    string details;
};

class Planner
{
private:
    vector<ExecutionStep> steps;
    string expressionToString(ExpressionNode* node);

public:
    void createPlan(QueryNode* query);
    void printPlan();
};

#endif