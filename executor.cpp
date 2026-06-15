#include "executor.h"

#include <iostream>
#include <cstdlib>
#include <algorithm>

using namespace std;

// -----------------------------
// Constructor
// -----------------------------
Executor::Executor(Database *database)
{
    db = database;
}

string Executor::getCellValue(
    const Row& row,
    const string& tableName,
    const string& columnName)
{
    return getCell(
        row,
        tableName,
        columnName).value;
}

int Executor::getCellInt(
    const Row& row,
    const string& tableName,
    const string& columnName)
{
    return stoi(
        getCell(
            row,
            tableName,
            columnName).value);
}

// -----------------------------
// MAIN EXPRESSION EVALUATION
// -----------------------------
bool Executor::evaluateExpression(
    const Row &row,
    const string &tableName,
    ExpressionNode *node)
{
    if (node == nullptr)
        return true;

    if (node->isLogical)
    {
        if (node->logicalOp == "AND")
        {
            return evaluateExpression(
                       row,
                       tableName,
                       node->left) &&
                   evaluateExpression(
                       row,
                       tableName,
                       node->right);
        }

        else if (node->logicalOp == "OR")
        {
            return evaluateExpression(
                       row,
                       tableName,
                       node->left) ||
                   evaluateExpression(
                       row,
                       tableName,
                       node->right);
        }

        return false;
    }

    return evaluateLeafCondition(
        row,
        tableName,
        node);
}

// -----------------------------
// LEAF CONDITION EVALUATION
// -----------------------------
bool Executor::evaluateLeafCondition(
    const Row &row,
    const string &tableName,
    ExpressionNode *node)
{
    string column = node->column;
    string op = node->op;
    string value = node->value;

    // STRING
    if (node->valueType == DataType::STRING)
    {
        string leftValue;

        leftValue =
            getCellValue(
                row,
                tableName,
                column);

        if (value.size() >= 2 &&
            value.front() == '\'' &&
            value.back() == '\'')
        {
            value =
                value.substr(
                    1,
                    value.size() - 2);
        }

        if (op == "=")
            return leftValue == value;

        if (op == "!=")
            return leftValue != value;

        return false;
    }

    // INT
    if (node->valueType == DataType::INT)
    {
        int leftValue = 0;

        leftValue =
            getCellInt(
                row,
                tableName,
                column);

        int rightValue = stoi(value);

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

    return false;
}

// -----------------------------
// EXECUTION PIPELINE
// -----------------------------
void Executor::execute(QueryNode *query)
{
    Table *table =
        db->getTable(
            query->table->tableName);

    if (!table)
    {
        cout << "Table not found" << endl;
        return;
    }

    // -------------------------
    // FILTER PHASE
    // -------------------------

    vector<Row> resultRows;

    for (const auto &row : table->rows)
    {
        if (!evaluateExpression(
                row,
                query->table->tableName,
                query->whereExpression))
        {
            continue;
        }

        resultRows.push_back(row);
    }

    // -------------------------
    // SORT PHASE
    // -------------------------

    if (query->orderBy != nullptr)
    {
        string column =
            query->orderBy->column;

        bool ascending =
            (query->orderBy->direction == "ASC");

        sort(
            resultRows.begin(),
            resultRows.end(),
            [&](const Row &a,
                const Row &b)
            {
                int idx =
                    db->getColumnIndex(
                        query->table->tableName,
                        column);

                if (idx < 0)
                {
                    return false;
                }

                DataType type =
                    db->getColumnType(
                        query->table->tableName,
                        column);

                if (type == DataType::INT)
                {
                    int left =
                        stoi(a.values[idx].value);

                    int right =
                        stoi(b.values[idx].value);

                    return ascending
                               ? left < right
                               : left > right;
                }

                return ascending
                           ? a.values[idx].value <
                                 b.values[idx].value
                           : a.values[idx].value >
                                 b.values[idx].value;
            });
    }

    // -------------------------
    // LIMIT PHASE
    // -------------------------

    if (query->limit != nullptr)
    {
        size_t limitCount =
            static_cast<size_t>(
                query->limit->count);

        if (resultRows.size() > limitCount)
        {
            resultRows.resize(limitCount);
        }
    }

    // -------------------------
    // OUTPUT
    // -------------------------

    cout << "\nRESULT\n\n";

    if (query->columns->selectAll)
    {
        const TableSchema *schema =
            db->getSchema(
                query->table->tableName);

        for (const auto &column :
             schema->columns)
        {
            cout
                << column.name
                << "\t";
        }

        cout << endl;

        cout
            << "------------------------"
            << endl;
    }
    else
    {
        for (const auto &col :
             query->columns->columns)
        {
            cout << col << "\t";
        }

        cout << endl;

        cout
            << "------------------------"
            << endl;
    }

    // -------------------------
    // PRINT PHASE
    // -------------------------

    for (const auto &row : resultRows)
    {
        if (query->columns->selectAll)
        {
            for (const auto &cell :
                 row.values)
            {
                cout
                    << cell.value
                    << "\t";
            }

            cout << endl;
        }
        else
        {
            for (const auto &col :
                 query->columns->columns)
            {
                int idx =
                    db->getColumnIndex(
                        query->table->tableName,
                        col);

                if (idx >= 0)
                {
                    cout
                        << row.values[idx].value
                        << "\t";
                }
            }

            cout << endl;
        }
    }
}

void Executor::executeInsert(
    InsertNode *node)
{
    for (const auto &insertRow :
         node->rows)
    {
        Row row;

        for (const auto &value :
             insertRow.values)
        {
            Cell cell;

            cell.value = value.value;
            cell.type = value.type;

            if (cell.type == DataType::STRING)
            {
                if (cell.value.size() >= 2 &&
                    cell.value.front() == '\'' &&
                    cell.value.back() == '\'')
                {
                    cell.value =
                        cell.value.substr(
                            1,
                            cell.value.size() - 2);
                }
            }

            row.values.push_back(cell);
        }

        db->insertRow(
            node->tableName,
            row);
    }

    cout
        << node->rows.size()
        << " row(s) inserted"
        << endl;
}


Cell Executor::getCell(
    const Row& row,
    const string& tableName,
    const string& columnName)
{
    int idx =
        db->getColumnIndex(
            tableName,
            columnName);

    if (idx < 0 ||
        static_cast<size_t>(idx) >=
            row.values.size())
    {
        return {"", DataType::STRING};
    }

    return row.values[idx];
}

void Executor::executeUpdate(
    UpdateNode* node)
{
    Table* table =
        db->getTable(
            node->tableName);

    if (!table)
    {
        return;
    }

    int targetIndex =
        db->getColumnIndex(
            node->tableName,
            node->columnName);

    int updatedRows = 0;

    for (auto& row : table->rows)
    {
        if (!evaluateExpression(
                row,
                node->tableName,
                node->whereExpression))
        {
            continue;
        }

        string value =
            node->newValue;

        if (node->valueType ==
            DataType::STRING)
        {
            if (value.size() >= 2 &&
                value.front() == '\'' &&
                value.back() == '\'')
            {
                value =
                    value.substr(
                        1,
                        value.size() - 2);
            }
        }

        row.values[targetIndex].value =
            value;

        updatedRows++;
    }

    cout
        << updatedRows
        << " row(s) updated"
        << endl;
}