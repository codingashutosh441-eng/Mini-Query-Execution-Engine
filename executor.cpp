#include "executor.h"
#include "storage.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <unordered_set>

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

    vector<Row> sourceRows;

    if (!query->joins.empty())
    {
        const JoinNode &join =
            query->joins[0];

        sourceRows =
            executeJoin(
                join.type,
                query->table->tableName,
                join.rightTable,
                join.leftColumn,
                join.rightColumn);

        if (query->whereExpression)
        {
            vector<Row> filtered;

            for (const auto &row :
                 sourceRows)
            {
                if (evaluateJoinedLeafCondition(
                        row,
                        query,
                        query->whereExpression))
                {
                    filtered.push_back(row);
                }
            }

            sourceRows = filtered;
        }

        cout
            << "[JOIN ROWS: "
            << sourceRows.size()
            << "]"
            << endl;
    }
    else
    {
        sourceRows = table->rows;
    }

    // -------------------------
    // FILTER PHASE
    // -------------------------

    vector<Row> resultRows;

    bool usedIndex = false;
    if (!usedIndex &&
    query->whereExpression != nullptr &&
    query->whereExpression->isLogical &&
    query->whereExpression->logicalOp == "AND")
{
    ExpressionNode *expr =
        query->whereExpression;

    if (!expr->left->isLogical &&
        !expr->right->isLogical &&
        expr->left->op == "=" &&
        expr->right->op == "=")
    {
        vector<string> columns =
        {
            expr->left->column,
            expr->right->column
        };

        string leftValue =
            expr->left->value;

        string rightValue =
            expr->right->value;

        if (expr->left->valueType ==
            DataType::STRING)
        {
            if (leftValue.size() >= 2 &&
                leftValue.front() == '\'' &&
                leftValue.back() == '\'')
            {
                leftValue =
                    leftValue.substr(
                        1,
                        leftValue.size() - 2);
            }
        }

        if (expr->right->valueType ==
            DataType::STRING)
        {
            if (rightValue.size() >= 2 &&
                rightValue.front() == '\'' &&
                rightValue.back() == '\'')
            {
                rightValue =
                    rightValue.substr(
                        1,
                        rightValue.size() - 2);
            }
        }

        vector<string> values =
        {
            leftValue,
            rightValue
        };

        if (db->hasIndex(
                query->table->tableName,
                columns))
        {
            vector<Row> candidateRows =
                db->lookupIndex(
                    query->table->tableName,
                    columns,
                    values);

            for (const auto &row :
                 candidateRows)
            {
                if (evaluateExpression(
                        row,
                        query->table->tableName,
                        query->whereExpression))
                {
                    resultRows.push_back(row);
                }
            }

            usedIndex = true;

            cout
                << "[COMPOSITE INDEX USED]"
                << endl;
        }
    }
}

    if (!usedIndex &&
        query->whereExpression != nullptr &&
        !query->whereExpression->isLogical &&
        query->whereExpression->op == "=")
    {
        string column =
            query->whereExpression->column;

        string value =
            query->whereExpression->value;

        if (query->whereExpression->valueType ==
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

        if (db->hasIndex(
                query->table->tableName,
                {column}))
        {
            vector<Row> candidateRows =
                db->lookupIndex(
                    query->table->tableName,
                    {column},
                    {value});

            for (const auto &row : candidateRows)
            {
                if (evaluateExpression(
                        row,
                        query->table->tableName,
                        query->whereExpression))
                {
                    resultRows.push_back(row);
                }
            }

            usedIndex = true;

            cout
                << "[INDEX USED]"
                << endl;
        }
    }

    if (!usedIndex && query->joins.empty())
    {
        for (const auto &row : sourceRows)
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
    }

    if (!query->joins.empty())
    {
        resultRows = sourceRows;
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
                int idx;

                if (!query->joins.empty())
                {
                    idx =
                        findJoinedColumnIndex(
                            query,
                            column);
                }
                else
                {
                    idx =
                        db->getColumnIndex(
                            query->table->tableName,
                            column);
                }

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

    // -------------------------
    // DISTINCT PHASE
    // -------------------------

    if (query->columns->distinct)
    {
        unordered_set<string> seen;

        vector<Row> distinctRows;

        for (const auto &row : resultRows)
        {
            string key;

            if (query->columns->selectAll)
            {
                for (const auto &cell :
                     row.values)
                {
                    key += cell.value + "|";
                }
            }
            else
            {
                for (const auto &col :
                     query->columns->columns)
                {
                    int idx;

                    if (!query->joins.empty())
                    {
                        idx =
                            findJoinedColumnIndex(
                                query,
                                col);
                    }
                    else
                    {
                        idx =
                            db->getColumnIndex(
                                query->table->tableName,
                                col);
                    }

                    if (idx >= 0)
                    {
                        key +=
                            row.values[idx].value + "|";
                    }
                }
            }

            if (!seen.count(key))
            {
                seen.insert(key);

                distinctRows.push_back(row);
            }
        }

        resultRows = distinctRows;
    }

    cout << "\nRESULT\n\n";

    if (query->columns->selectAll)
    {
        if (!query->joins.empty())
        {
            auto columns =
                getJoinColumnNames(
                    query->table->tableName,
                    query->joins[0].rightTable);

            for (const auto &column :
                 columns)
            {
                cout
                    << column
                    << "\t";
            }
        }
        else
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
                int idx;

                if (!query->joins.empty())
                {
                    idx =
                        findJoinedColumnIndex(
                            query,
                            col);
                }
                else
                {
                    idx =
                        db->getColumnIndex(
                            query->table->tableName,
                            col);
                }

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
    int insertedRows = 0;
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

        if (!db->insertRow(
                node->tableName,
                row))
        {
            cout
                << db->getLastError()
                << endl;

            continue;
        }

        StorageManager::appendRow(
            node->tableName,
            row);

        insertedRows++;
    }

    cout
        << insertedRows
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
        db->rebuildIndexes();
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
        db->rebuildIndexes();
    }

    cout
        << deletedRows
        << " row(s) deleted"
        << endl;
}

int Executor::getJoinedCellInt(
    const Row &row,
    QueryNode *query,
    const string &columnName)
{
    int idx =
        findJoinedColumnIndex(
            query,
            columnName);

    if (idx < 0)
    {
        return 0;
    }

    string value =
        row.values[idx].value;

    if (value == "NULL")
    {
        return 0;
    }

    return stoi(value);
}

long long Executor::calculateCount(
    const vector<Row> &rows)
{
    return rows.size();
}

long long Executor::calculateSum(
    const vector<Row> &rows,
    QueryNode *query,
    const string &tableName,
    const string &column)
{
    long long total = 0;

    for (const auto &row : rows)
    {
        if (!query->joins.empty())
        {
            total +=
                getJoinedCellInt(
                    row,
                    query,
                    column);
        }
        else
        {
            total +=
                getCellInt(
                    row,
                    tableName,
                    column);
        }
    }
    return total;
}

double Executor::calculateAvg(
    const vector<Row> &rows,
    QueryNode *query,
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
            query,
            tableName,
            column);

    return static_cast<double>(total) / rows.size();
}

string Executor::calculateMin(
    const vector<Row> &rows,
    QueryNode *query,
    const string &tableName,
    const string &column)
{
    if (rows.empty())
    {
        return "";
    }

    int minValue;

    if (!query->joins.empty())
    {
        minValue =
            getJoinedCellInt(
                rows[0],
                query,
                column);
    }
    else
    {
        minValue =
            getCellInt(
                rows[0],
                tableName,
                column);
    }

    for (const auto &row : rows)
    {
        int value;

        if (!query->joins.empty())
        {
            value =
                getJoinedCellInt(
                    row,
                    query,
                    column);
        }
        else
        {
            value =
                getCellInt(
                    row,
                    tableName,
                    column);
        }

        minValue =
            min(minValue, value);
    }

    return to_string(minValue);
}

string Executor::calculateMax(
    const vector<Row> &rows,
    QueryNode *query,
    const string &tableName,
    const string &column)
{
    if (rows.empty())
    {
        return "";
    }

    int maxValue;

    if (!query->joins.empty())
    {
        maxValue =
            getJoinedCellInt(
                rows[0],
                query,
                column);
    }
    else
    {
        maxValue =
            getCellInt(
                rows[0],
                tableName,
                column);
    }

    for (const auto &row : rows)
    {
        int value;

        if (!query->joins.empty())
        {
            value =
                getJoinedCellInt(
                    row,
                    query,
                    column);
        }
        else
        {
            value =
                getCellInt(
                    row,
                    tableName,
                    column);
        }

        maxValue =
            max(maxValue, value);
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
                        query,
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
                        query,
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
                        query,
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
                        query,
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
    // string groupColumn =query->groupBy->columns[0];

    unordered_map<
        string,
        vector<Row>>
        groups;

    vector<GroupResult> results;

    for (const auto &row : rows)
    {
        string key;

        for (const auto &column :
             query->groupBy->columns)
        {
            string value;

            if (!query->joins.empty())
            {
                int idx =
                    findJoinedColumnIndex(
                        query,
                        column);

                if (idx >= 0)
                {
                    value =
                        row.values[idx].value;
                }
            }
            else
            {
                value =
                    getCellValue(
                        row,
                        tableName,
                        column);
            }

            key += value;
            key += "|";
        }

        groups[key].push_back(row);
    }

    cout << "\nRESULT\n\n";

    for (const auto &column :
         query->groupBy->columns)
    {
        cout
            << column
            << "\t";
    }

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

        stringstream ss(pair.first);

        string value;

        while (getline(
            ss,
            value,
            '|'))
        {
            if (!value.empty())
            {
                result.groupValues.push_back(
                    value);
            }
        }

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
                            query,
                            tableName,
                            agg.column)));
                break;

            case AggregateType::AVG:
                result.aggregateValues.push_back(
                    to_string(
                        calculateAvg(
                            groupRows,
                            query,
                            tableName,
                            agg.column)));
                break;

            case AggregateType::MIN:
                result.aggregateValues.push_back(
                    calculateMin(
                        groupRows,
                        query,
                        tableName,
                        agg.column));
                break;

            case AggregateType::MAX:
                result.aggregateValues.push_back(
                    calculateMax(
                        groupRows,
                        query,
                        tableName,
                        agg.column));
                break;

                // cout << "\t";
            }
        }
        results.push_back(result);
        // cout << "DEBUG RESULT ADDED" << endl;
    }

    if (query->having != nullptr)
    {
        vector<GroupResult> filtered;

        for (const auto &result : results)
        {
            if (passesHaving(
                    result,
                    query))
            {
                filtered.push_back(result);
            }
        }

        results = filtered;
    }

    // if (query->orderBy != nullptr)
    // {
    //     bool ascending =
    //         query->orderBy->direction == "ASC";

    //     sort(
    //         results.begin(),
    //         results.end(),
    //         [&](const GroupResult &a,
    //             const GroupResult &b)
    //         {
    //             int left =
    //                 stoi(a.groupKey);

    //             int right =
    //                 stoi(b.groupKey);

    //             return ascending
    //                        ? left < right
    //                        : left > right;
    //         });
    // }

    for (const auto &result :
         results)
    {
        for (const auto &value :
             result.groupValues)
        {
            cout
                << value
                << "\t";
        }

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

bool Executor::passesHaving(
    const GroupResult &result,
    QueryNode *query)
{
    if (query->having == nullptr)
    {
        return true;
    }

    if (result.aggregateValues.empty())
    {
        return false;
    }

    double left =
        stod(result.aggregateValues[0]);

    double right =
        stod(query->having->value);

    string op =
        query->having->op;

    if (op == "=")
        return left == right;

    if (op == "!=")
        return left != right;

    if (op == ">")
        return left > right;

    if (op == "<")
        return left < right;

    if (op == ">=")
        return left >= right;

    if (op == "<=")
        return left <= right;

    return false;
}

vector<Row> Executor::executeJoin(
    JoinType type,
    const string &leftTable,
    const string &rightTable,
    const string &leftColumn,
    const string &rightColumn)
{
    vector<Row> result;

    Table *left =
        db->getTable(leftTable);

    Table *right =
        db->getTable(rightTable);

    if (!left || !right)
    {
        return result;
    }

    int leftIdx =
        db->getColumnIndex(
            leftTable,
            leftColumn);

    int rightIdx =
        db->getColumnIndex(
            rightTable,
            rightColumn);

    if (type == JoinType::RIGHT)
    {
        size_t leftCols =
            left->rows[0].values.size();

        for (const auto &rightRow : right->rows)
        {
            bool matched = false;

            for (const auto &leftRow : left->rows)
            {
                if (leftRow.values[leftIdx].value ==
                    rightRow.values[rightIdx].value)
                {
                    matched = true;

                    Row merged;

                    merged.values =
                        leftRow.values;

                    merged.values.insert(
                        merged.values.end(),
                        rightRow.values.begin(),
                        rightRow.values.end());

                    result.push_back(merged);
                }
            }

            if (!matched)
            {
                Row merged;

                for (size_t i = 0;
                     i < leftCols;
                     i++)
                {
                    merged.values.push_back(
                        {"NULL",
                         DataType::STRING});
                }

                merged.values.insert(
                    merged.values.end(),
                    rightRow.values.begin(),
                    rightRow.values.end());

                result.push_back(merged);
            }
        }

        return result;
    }

    for (const auto &leftRow : left->rows)
    {
        bool matched = false;

        for (const auto &rightRow : right->rows)
        {
            if (leftRow.values[leftIdx].value ==
                rightRow.values[rightIdx].value)
            {
                matched = true;

                Row merged;

                merged.values =
                    leftRow.values;

                merged.values.insert(
                    merged.values.end(),
                    rightRow.values.begin(),
                    rightRow.values.end());

                result.push_back(merged);
            }
        }

        if (!matched &&
            type == JoinType::LEFT)
        {
            Row merged;

            merged.values =
                leftRow.values;

            for (size_t i = 0;
                 i < right->rows[0].values.size();
                 i++)
            {
                merged.values.push_back(
                    {"NULL",
                     DataType::STRING});
            }

            result.push_back(merged);
        }
    }

    return result;
}

vector<string> Executor::getJoinColumnNames(
    const string &leftTable,
    const string &rightTable)
{
    vector<string> columns;

    const TableSchema *leftSchema =
        db->getSchema(leftTable);

    const TableSchema *rightSchema =
        db->getSchema(rightTable);

    if (leftSchema)
    {
        for (const auto &column :
             leftSchema->columns)
        {
            columns.push_back(
                column.name);
        }
    }

    if (rightSchema)
    {
        for (const auto &column :
             rightSchema->columns)
        {
            columns.push_back(
                column.name);
        }
    }

    return columns;
}

int Executor::findJoinedColumnIndex(
    QueryNode *query,
    const string &columnName)
{
    int index = 0;

    const TableSchema *leftSchema =
        db->getSchema(
            query->table->tableName);

    if (leftSchema)
    {
        for (const auto &column :
             leftSchema->columns)
        {
            if (column.name == columnName)
            {
                return index;
            }

            index++;
        }
    }

    const TableSchema *rightSchema =
        db->getSchema(
            query->joins[0].rightTable);

    if (rightSchema)
    {
        for (const auto &column :
             rightSchema->columns)
        {
            if (column.name == columnName)
            {
                return index;
            }

            index++;
        }
    }

    return -1;
}

bool Executor::evaluateJoinedLeafCondition(
    const Row &row,
    QueryNode *query,
    ExpressionNode *node)
{
    int idx =
        findJoinedColumnIndex(
            query,
            node->column);

    if (idx < 0)
    {
        return false;
    }

    string cellValue =
        row.values[idx].value;

    if (node->valueType == DataType::INT)
    {
        int left =
            stoi(cellValue);

        int right =
            stoi(node->value);

        if (node->op == "=")
            return left == right;

        if (node->op == "!=")
            return left != right;

        if (node->op == ">")
            return left > right;

        if (node->op == "<")
            return left < right;

        if (node->op == ">=")
            return left >= right;

        if (node->op == "<=")
            return left <= right;
    }

    return false;
}