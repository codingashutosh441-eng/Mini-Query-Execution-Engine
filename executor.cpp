#include "executor.h"
#include <iostream>
#include <cstdlib>

using namespace std;

// -----------------------------
// Constructor
// -----------------------------
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
    string value = node->value;

    // =========================
    // STRING HANDLING
    // =========================
    if (node->valueType == DataType::STRING)
    {
        string leftValue;

        if (column == "name")
            leftValue = row.name;
        else
            return false;

        // remove quotes from literal
        if (value.size() >= 2 &&
            value.front() == '\'' &&
            value.back() == '\'')
        {
            value = value.substr(1, value.size() - 2);
        }

        if (op == "=")  return leftValue == value;
        if (op == "!=") return leftValue != value;

        return false;
    }

    // =========================
    // INTEGER HANDLING
    // =========================
    if (node->valueType == DataType::INT)
    {
        int leftValue = 0;

        if (column == "id")
            leftValue = row.id;
        else if (column == "age")
            leftValue = row.age;
        else
            return false;

        int rightValue = stoi(value);

        if (op == "=")  return leftValue == rightValue;
        if (op == "!=") return leftValue != rightValue;
        if (op == ">")  return leftValue > rightValue;
        if (op == "<")  return leftValue < rightValue;
        if (op == ">=") return leftValue >= rightValue;
        if (op == "<=") return leftValue <= rightValue;

        return false;
    }

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