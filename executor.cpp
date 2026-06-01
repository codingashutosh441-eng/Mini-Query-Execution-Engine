#include "executor.h"
#include <iostream>
#include <cstdlib>

using namespace std;

Executor::Executor(Database* database)
{
    db = database;
}

// -----------------------------
// MAIN EXPRESSION EVALUATION
// -----------------------------
bool Executor::evaluateExpression(Row row, ExpressionNode* node)
{
    if (node == nullptr)
        return true;

    // LOGICAL NODE
    if (node->isLogical)
    {
        if (node->logicalOp == "AND")
        {
            return evaluateExpression(row, node->left) &&
                   evaluateExpression(row, node->right);
        }
        else if (node->logicalOp == "OR")
        {
            return evaluateExpression(row, node->left) ||
                   evaluateExpression(row, node->right);
        }

        return false;
    }

    // LEAF NODE
    return evaluateLeafCondition(row, node);
}

// -----------------------------
// LEAF CONDITION EVALUATION
// -----------------------------
bool Executor::evaluateLeafCondition(Row row, ExpressionNode* node)
{
    string column = node->column;
    string op = node->op;
    string right = node->value;

    // -------------------------
    // STRING COLUMN HANDLING
    // -------------------------
    if (column == "name")
    {
        string leftValue = row.name;

        // remove quotes from string literal
        if (right.size() >= 2 &&
            right.front() == '\'' &&
            right.back() == '\'')
        {
            right = right.substr(1, right.size() - 2);
        }

        if (op == "=")
            return leftValue == right;

        if (op == "!=")
            return leftValue != right;

        // invalid operator for string
        return false;
    }

    // -------------------------
    // INTEGER COLUMN HANDLING
    // -------------------------
    int leftValue = 0;

    if (column == "id")
        leftValue = row.id;
    else if (column == "age")
        leftValue = row.age;
    else
        return false;

    int rightValue = 0;

    try
    {
        rightValue = stoi(right);
    }
    catch (...)
    {
        return false;
    }

    // -------------------------
    // NUMERIC OPERATORS
    // -------------------------
    if (op == "=")
        return leftValue == rightValue;

    if (op == "!=")
        return leftValue != rightValue;

    if (op == ">")
        return leftValue > rightValue;

    if (op == "<")
        return leftValue < rightValue;

    if (op == ">=")
        return leftValue >= rightValue;

    if (op == "<=")
        return leftValue <= rightValue;

    return false;
}

// -----------------------------
// EXECUTION PIPELINE
// -----------------------------
void Executor::execute(QueryNode* query)
{
    Table* table = db->getTable(query->table->tableName);

    if (!table)
    {
        cout << "Table not found" << endl;
        return;
    }

    cout << "\nRESULT\n\n";

    // -------------------------
    // HEADER
    // -------------------------
    if (query->columns->selectAll)
    {
        cout << "id\tname\tage" << endl;
        cout << "------------------------" << endl;
    }
    else
    {
        for (const auto& col : query->columns->columns)
        {
            cout << col << "\t";
        }
        cout << endl;
        cout << "------------------------" << endl;
    }

    // -------------------------
    // ROW PROCESSING
    // -------------------------
    for (const auto& row : table->rows)
    {
        if (!evaluateExpression(row, query->whereExpression))
            continue;

        // SELECT *
        if (query->columns->selectAll)
        {
            cout << row.id << "\t"
                 << row.name << "\t"
                 << row.age << endl;
        }
        else
        {
            for (const auto& col : query->columns->columns)
            {
                if (col == "id")
                    cout << row.id << "\t";
                else if (col == "name")
                    cout << row.name << "\t";
                else if (col == "age")
                    cout << row.age << "\t";
            }
            cout << endl;
        }
    }
}