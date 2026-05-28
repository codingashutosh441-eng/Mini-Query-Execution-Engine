#include "planner.h"

void Planner::createPlan(QueryNode* query)
{
    steps.clear();

    if(query == nullptr)
    {
        return;
    }

    if(query->table != nullptr)
    {
        ExecutionStep step;

        step.type = StepType::SCAN;

        step.details =
            query->table->tableName;

        steps.push_back(step);
    }

    if(query->condition != nullptr)
    {
        ExecutionStep step;

        step.type = StepType::FILTER;

        step.details =
            query->condition->left + " " +
            query->condition->op + " " +
            query->condition->right;

        steps.push_back(step);
    }

    if(query->columns != nullptr)
    {
        if(!query->columns->selectAll)
        {
            ExecutionStep step;

            step.type = StepType::PROJECT;

            for(string column :
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

    for(const ExecutionStep& step : steps)
    {
        cout << stepNumber << ". ";

        if(step.type == StepType::SCAN)
        {
            cout << "SCAN TABLE "
                 << step.details;
        }

        else if(step.type == StepType::FILTER)
        {
            cout << "FILTER rows where "
                 << step.details;
        }

        else if(step.type == StepType::PROJECT)
        {
            cout << "PROJECT columns: "
                 << step.details;
        }

        cout << endl << endl;

        stepNumber++;
    }
}