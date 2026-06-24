#include "planner.h"

Planner::Planner(
    Database *database)
{
    db = database;
}

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
        bool useIndex = false;

        string indexDetails;

        if (query->whereExpression != nullptr)
        {
            ExpressionNode *expr =
                query->whereExpression;

            // Single-column index
            if (!expr->isLogical &&
                expr->op == "=")
            {
                if (db->hasIndex(
                        query->table->tableName,
                        {expr->column}))
                {
                    useIndex = true;

                    indexDetails =
                        query->table->tableName +
                        "." +
                        expr->column;
                }
            }

            // Composite index
            else if (
                expr->isLogical &&
                expr->logicalOp == "AND")
            {
            

                if (!expr->left->isLogical &&
                    !expr->right->isLogical &&
                    expr->left->op == "=" &&
                    expr->right->op == "=")
                {
                    vector<string> columns =
                        {
                            expr->left->column,
                            expr->right->column};

                    if (db->hasIndex(
                            query->table->tableName,
                            columns))
                    {
                        useIndex = true;

                        indexDetails =
                            query->table->tableName;

                        for (const auto &col :
                             columns)
                        {
                            indexDetails +=
                                "." + col;
                        }
                    }
                }
            }
        }

        ExecutionStep step;

        if (useIndex)
        {
            step.type =
                StepType::INDEX_SEEK;

            step.details = indexDetails;
        }
        else
        {
            step.type =
                StepType::SCAN;

            step.details =
                query->table->tableName;
        }

        steps.push_back(step);
    }

    if (query->columns != nullptr &&
        query->columns->distinct)
    {
        ExecutionStep step;

        step.type = StepType::DISTINCT;

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

    if (query->columns != nullptr &&
        !query->columns->aggregates.empty())
    {
        ExecutionStep step;

        step.type =
            StepType::AGGREGATE;

        for (const auto &agg :
             query->columns->aggregates)
        {
            switch (agg.type)
            {
            case AggregateType::COUNT:
                step.details +=
                    "COUNT(*) ";
                break;

            case AggregateType::SUM:
                step.details +=
                    "SUM(" +
                    agg.column +
                    ") ";
                break;

            case AggregateType::AVG:
                step.details +=
                    "AVG(" +
                    agg.column +
                    ") ";
                break;

            case AggregateType::MIN:
                step.details +=
                    "MIN(" +
                    agg.column +
                    ") ";
                break;

            case AggregateType::MAX:
                step.details +=
                    "MAX(" +
                    agg.column +
                    ") ";
                break;
            }
        }

        steps.push_back(step);
    }

    if (query->groupBy != nullptr)
    {
        ExecutionStep step;

        step.type =
            StepType::GROUP_BY;

        for (const auto &col :
             query->groupBy->columns)
        {
            step.details +=
                col + " ";
        }

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
        if (!query->columns->selectAll && !query->columns->columns.empty())
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
        else if (step.type == StepType::DISTINCT)
        {
            cout << "DISTINCT";
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
        else if (step.type == StepType::GROUP_BY)
        {
            cout << "GROUP BY "
                 << step.details;
        }
        else if (step.type == StepType::AGGREGATE)
        {
            cout << "AGGREGATE "
                 << step.details;
        }
        else if (
            step.type ==
            StepType::INDEX_SEEK)
        {
            cout
                << "INDEX SEEK "
                << step.details;
        }

        cout << endl
             << endl;

        stepNumber++;
    }
}