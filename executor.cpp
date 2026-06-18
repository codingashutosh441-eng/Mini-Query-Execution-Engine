#include "executor.h"
#include "storage.h"
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
    const Row &row,
    const string &tableName,
    const string &columnName)
{
    return getCell(
               row,
               tableName,
               columnName)
        .value;
}

int Executor::getCellInt(
    const Row &row,
    const string &tableName,
    const string &columnName)
{
    return stoi(
        getCell(
            row,
            tableName,
            columnName)
            .value);
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

    if (!query->columns->aggregates.empty())
    {
        // cout << "DEBUG: aggregate found" << endl;
        if (query->groupBy != nullptr)
        {
            // cout << "DEBUG: group by found" << endl;
            executeGroupBy(
                resultRows,
                query->table->tableName,
                query);
        }
        else
        {
            // cout << "DEBUG: no group by" << endl;
            executeAggregate(
                resultRows,
                query->table->tableName,
                query);
        }

        return;
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

        StorageManager::appendRow(
            node->tableName,
            row);
    }

    cout
        << node->rows.size()
        << " row(s) inserted"
        << endl;
}

Cell Executor::getCell(
    const Row &row,
    const string &tableName,
    const string &columnName)
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
    UpdateNode *node)
{
    Table *table =
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

    for (auto &row : table->rows)
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

    const TableSchema *schema =
        db->getSchema(
            node->tableName);

    if (schema)
    {
        StorageManager::saveTable(
            *table);
    }

    cout
        << updatedRows
        << " row(s) updated"
        << endl;
}

void Executor::executeDelete(
    DeleteNode *node)
{
    Table *table =
        db->getTable(
            node->tableName);

    if (!table)
    {
        return;
    }

    vector<Row> remainingRows;

    int deletedRows = 0;

    for (const auto &row :
         table->rows)
    {
        bool shouldDelete =
            evaluateExpression(
                row,
                node->tableName,
                node->whereExpression);

        if (shouldDelete)
        {
            deletedRows++;
        }
        else
        {
            remainingRows.push_back(
                row);
        }
    }

    table->rows =
        remainingRows;

    const TableSchema *schema =
        db->getSchema(
            node->tableName);

    if (schema)
    {
        StorageManager::saveTable(
            *table);
    }

    cout
        << deletedRows
        << " row(s) deleted"
        << endl;
}

long long Executor::calculateCount(
    const vector<Row> &rows)
{
    return rows.size();
}

long long Executor::calculateSum(
    const vector<Row> &rows,
    const string &tableName,
    const string &column)
{
    long long total = 0;

    for (const auto &row : rows)
    {
        total +=
            getCellInt(
                row,
                tableName,
                column);
    }

    return total;
}

double Executor::calculateAvg(
    const vector<Row> &rows,
    const string &tableName,
    const string &column)
{
    if (rows.empty())
    {
        return 0;
    }

    long long total =
        calculateSum(
            rows,
            tableName,
            column);

    return static_cast<double>(total) / rows.size();
}

string Executor::calculateMin(
    const vector<Row> &rows,
    const string &tableName,
    const string &column)
{
    if (rows.empty())
    {
        return "";
    }

    int minValue =
        getCellInt(
            rows[0],
            tableName,
            column);

    for (const auto &row : rows)
    {
        minValue =
            min(
                minValue,
                getCellInt(
                    row,
                    tableName,
                    column));
    }

    return to_string(minValue);
}

string Executor::calculateMax(
    const vector<Row> &rows,
    const string &tableName,
    const string &column)
{
    if (rows.empty())
    {
        return "";
    }

    int maxValue =
        getCellInt(
            rows[0],
            tableName,
            column);

    for (const auto &row : rows)
    {
        maxValue =
            max(
                maxValue,
                getCellInt(
                    row,
                    tableName,
                    column));
    }

    return to_string(maxValue);
}

void Executor::executeAggregate(
    const vector<Row> &rows,
    const string &tableName,
    QueryNode *query)
{
    cout << "\nRESULT\n\n";

    for (const auto &agg :
         query->columns->aggregates)
    {
        switch (agg.type)
        {
        case AggregateType::COUNT:
        {
            cout << "COUNT(*)"
                 << endl;

            cout << calculateCount(rows)
                 << endl;

            break;
        }

        case AggregateType::SUM:
        {
            cout << "SUM("
                 << agg.column
                 << ")"
                 << endl;

            cout << calculateSum(
                        rows,
                        tableName,
                        agg.column)
                 << endl;

            break;
        }

        case AggregateType::AVG:
        {
            cout << "AVG("
                 << agg.column
                 << ")"
                 << endl;

            cout << calculateAvg(
                        rows,
                        tableName,
                        agg.column)
                 << endl;

            break;
        }

        case AggregateType::MIN:
        {
            cout << "MIN("
                 << agg.column
                 << ")"
                 << endl;

            cout << calculateMin(
                        rows,
                        tableName,
                        agg.column)
                 << endl;

            break;
        }

        case AggregateType::MAX:
        {
            cout << "MAX("
                 << agg.column
                 << ")"
                 << endl;

            cout << calculateMax(
                        rows,
                        tableName,
                        agg.column)
                 << endl;

            break;
        }
        }
    }
}

void Executor::executeGroupBy(
    const vector<Row> &rows,
    const string &tableName,
    QueryNode *query)
{
    string groupColumn =
        query->groupBy->columns[0];

    unordered_map<
        string,
        vector<Row>>
        groups;

    vector<GroupResult> results;

    for (const auto &row : rows)
    {
        string key =
            getCellValue(
                row,
                tableName,
                groupColumn);

        groups[key].push_back(row);
    }

    cout << "\nRESULT\n\n";

    cout << groupColumn << "\t";

    for (const auto &agg :
         query->columns->aggregates)
    {
        switch (agg.type)
        {
        case AggregateType::COUNT:
            cout << "COUNT(*)";
            break;

        case AggregateType::SUM:
            cout << "SUM(" << agg.column << ")";
            break;

        case AggregateType::AVG:
            cout << "AVG(" << agg.column << ")";
            break;

        case AggregateType::MIN:
            cout << "MIN(" << agg.column << ")";
            break;

        case AggregateType::MAX:
            cout << "MAX(" << agg.column << ")";
            break;
        }

        cout << "\t";
    }

    cout << endl;

    cout << "------------------------"
         << endl;

    for (auto &pair : groups)
    {

        GroupResult result;

        result.groupKey = pair.first;

        // cout << pair.first << "\t";

        const vector<Row> &groupRows =
            pair.second;

        for (const auto &agg :
             query->columns->aggregates)
        {
            switch (agg.type)
            {
            case AggregateType::COUNT:
                result.aggregateValues.push_back(
                    to_string(
                        calculateCount(groupRows)));
                break;

            case AggregateType::SUM:
                result.aggregateValues.push_back(
                    to_string(
                        calculateSum(
                            groupRows,
                            tableName,
                            agg.column)));
                break;

            case AggregateType::AVG:
                result.aggregateValues.push_back(
                    to_string(
                        calculateAvg(
                            groupRows,
                            tableName,
                            agg.column)));
                break;

            case AggregateType::MIN:
                result.aggregateValues.push_back(
                    calculateMin(
                        groupRows,
                        tableName,
                        agg.column));
                break;

            case AggregateType::MAX:
                result.aggregateValues.push_back(
                    calculateMax(
                        groupRows,
                        tableName,
                        agg.column));
                break;

                // cout << "\t";
            }
        }
        results.push_back(result);
        //cout << "DEBUG RESULT ADDED" << endl;
    }

    if (query->orderBy != nullptr)
    {
        bool ascending =
            query->orderBy->direction == "ASC";

        sort(
            results.begin(),
            results.end(),
            [&](const GroupResult &a,
                const GroupResult &b)
            {
                int left =
                    stoi(a.groupKey);

                int right =
                    stoi(b.groupKey);

                return ascending
                           ? left < right
                           : left > right;
            });
    }

    for (const auto &result :
         results)
    {
        cout
            << result.groupKey
            << "\t";

        for (const auto &value :
             result.aggregateValues)
        {
            cout
                << value
                << "\t";
        }

        cout << endl;
    }
}