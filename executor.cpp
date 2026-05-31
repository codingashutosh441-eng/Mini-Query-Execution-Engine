#include "executor.h"
#include <iostream>

using namespace std;

Executor::Executor(Database* database)
{
    db = database;
}

// MAIN EXPRESSION EVALUATION
bool Executor::evaluateExpression(
    Row row,
    ExpressionNode* node)
{
    if(node == nullptr)
    {
        return true;
    }

    // LOGICAL NODE
    if(node->isLogical)
    {
        if(node->logicalOp == "AND")
        {
            return evaluateExpression(row, node->left) &&
                   evaluateExpression(row, node->right);
        }

        else if(node->logicalOp == "OR")
        {
            return evaluateExpression(row, node->left) ||
                   evaluateExpression(row, node->right);
        }

        return false;
    }

    // LEAF NODE
    return evaluateLeafCondition(row, node);
}

// LEAF CONDITION EVALUATION
bool Executor::evaluateLeafCondition(
    Row row,
    ExpressionNode* node)
{
    string left = node->column;
    string op   = node->op;
    string right = node->value;

    // STRING COLUMN
    if(left == "name")
    {
        string rowValue = row.name;
        string compareValue = right;

        // remove quotes if present
        if(compareValue.size() >= 2 &&
           compareValue.front() == '\'' &&
           compareValue.back() == '\'')
        {
            compareValue = compareValue.substr(
                1,
                compareValue.size() - 2);
        }

        if(op == "==")
        {
            return rowValue == compareValue;
        }

        else if(op == "!=")
        {
            return rowValue != compareValue;
        }

        return false;
    }

    // INTEGER COLUMNS
    int rowValue = 0;
    int compareValue = stoi(right);

    if(left == "age")
    {
        rowValue = row.age;
    }

    else if(left == "id")
    {
        rowValue = row.id;
    }

    if(op == ">")
    {
        return rowValue > compareValue;
    }

    else if(op == "<")
    {
        return rowValue < compareValue;
    }

    else if(op == ">=")
    {
        return rowValue >= compareValue;
    }

    else if(op == "<=")
    {
        return rowValue <= compareValue;
    }

    else if(op == "==")
    {
        return rowValue == compareValue;
    }

    else if(op == "!=")
    {
        return rowValue != compareValue;
    }

    return false;
}

// -----------------------------
// EXECUTION PIPELINE
// -----------------------------
void Executor::execute(QueryNode* query)
{
    Table* table =
        db->getTable(query->table->tableName);

    if(table == nullptr)
    {
        cout << "Table not found" << endl;
        return;
    }

    cout << "\nRESULT\n\n";

    // HEADERS
    if(query->columns->selectAll)
    {
        cout << "id\tname\tage" << endl;
        cout << "------------------------" << endl;
    }
    else
    {
        for(const auto& column : query->columns->columns)
        {
            cout << column << "\t";
        }
        cout << endl;
        cout << "------------------------" << endl;
    }

    // ROWS
    for(const auto& row : table->rows)
    {
        // FILTER
        if(!evaluateExpression(row, query->whereExpression))
        {
            continue;
        }

        // SELECT *
        if(query->columns->selectAll)
        {
            cout << row.id << "\t"
                 << row.name << "\t"
                 << row.age << endl;
        }

        // PROJECT COLUMNS
        else
        {
            for(const auto& column : query->columns->columns)
            {
                if(column == "id")
                {
                    cout << row.id << "\t";
                }
                else if(column == "name")
                {
                    cout << row.name << "\t";
                }
                else if(column == "age")
                {
                    cout << row.age << "\t";
                }
            }
            cout << endl;
        }
    }
}