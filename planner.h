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
    PROJECT
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

public:

    void createPlan(QueryNode* query);

    void printPlan();
};

#endif