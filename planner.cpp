#include "planner.h"

string Planner::expressionToString(
    ExpressionNode *node)
{
    if (node == nullptr)
    {
        return "";
    }

    if (!node->isLogical)
    {
        return node->column +
               " " +
               node->op +
               " " +
               node->value;
    }

    return "(" +
           expressionToString(node->left) +
           " " +
           node->logicalOp +
           " " +
           expressionToString(node->right) +
           ")";
}

void Planner::createPlan(QueryNode *query)
{
    steps.clear();

    if (query == nullptr)
    {
        return;
    }

    if (query->table != nullptr)
    {
        ExecutionStep step;

        step.type = StepType::SCAN;

        step.details =
            query->table->tableName;

        steps.push_back(step);
    }

    if (query->whereExpression != nullptr)
    {
        ExecutionStep step;

        step.type = StepType::FILTER;

        step.details =
            expressionToString(
                query->whereExpression);

        steps.push_back(step);
    }

    if (query->orderBy != nullptr)
    {
        ExecutionStep step;
        step.type = StepType::SORT;

        step.details =
            query->orderBy->column +
            " " +
            query->orderBy->direction;

        steps.push_back(step);
    }

    if (query->limit != nullptr)
    {
        ExecutionStep step;

        step.type = StepType::LIMIT;

        step.details =
            to_string(
                query->limit->count);

        steps.push_back(step);
    }

    if (query->columns != nullptr)
    {
        if (!query->columns->selectAll)
        {
            ExecutionStep step;

            step.type = StepType::PROJECT;

            for (string column :
                 query->columns->columns)
            {
                step.details +=
                    column + " ";
            }

            steps.push_back(step);
        }
    }
}

void Planner::printPlan()
{
    cout << "\nEXECUTION PLAN\n\n";

    int stepNumber = 1;

    for (const ExecutionStep &step : steps)
    {
        cout << stepNumber << ". ";

        if (step.type == StepType::SCAN)
        {
            cout << "SCAN TABLE "
                 << step.details;
        }

        else if (step.type == StepType::FILTER)
        {
            cout << "FILTER rows where "
                 << step.details;
        }
        else if (step.type == StepType::SORT)
        {
            cout << "SORT BY "
                 << step.details;
        }

        else if (step.type == StepType::LIMIT)
        {
            cout << "LIMIT "
                 << step.details;
        }

        else if (step.type == StepType::PROJECT)
        {
            cout << "PROJECT columns: "
                 << step.details;
        }

        cout << endl
             << endl;

        stepNumber++;
    }
}