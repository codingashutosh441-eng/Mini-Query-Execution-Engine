#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "database.h"
#include "ast.h"
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

struct GroupResult
{
    vector<string> groupValues;

    vector<string> aggregateValues;
};

class Executor
{
private:
    Database *db;

    // Evaluate full expression tree (AND / OR / leaf)
    bool evaluateExpression(
        const Row &row,
        const string &tableName,
        ExpressionNode *node);

    bool evaluateLeafCondition(
        const Row &row,
        const string &tableName,
        ExpressionNode *node);
    string getCellValue(
        const Row &row,
        const string &tableName,
        const string &columnName);

    int getCellInt(
        const Row &row,
        const string &tableName,
        const string &columnName);

    Cell getCell(
        const Row &row,
        const string &tableName,
        const string &columnName);

    void executeAggregate(
        const vector<Row> &rows,
        const string &tableName,
        QueryNode *query);

    void executeGroupBy(
        const vector<Row> &rows,
        const string &tableName,
        QueryNode *query);

    bool passesHaving(
        const GroupResult &result,
        QueryNode *query);

    long long calculateCount(
        const vector<Row> &rows);

    long long calculateSum(
        const vector<Row> &rows,
        QueryNode *query,
        const string &tableName,
        const string &column);

    double calculateAvg(
        const vector<Row> &rows,
        QueryNode *query,
        const string &tableName,
        const string &column);

    string calculateMin(
        const vector<Row> &rows,
        QueryNode *query,
        const string &tableName,
        const string &column);

    string calculateMax(
        const vector<Row> &rows,
        QueryNode *query,
        const string &tableName,
        const string &column);

    vector<Row> executeJoin(
        JoinType type,
        const string &leftTable,
        const string &rightTable,
        const string &leftColumn,
        const string &rightColumn);

    vector<string> getJoinColumnNames(
        const string &leftTable,
        const string &rightTable);

    int findJoinedColumnIndex(
        QueryNode *query,
        const string &columnName);

    bool evaluateJoinedLeafCondition(
        const Row &row,
        QueryNode *query,
        ExpressionNode *node);
    int getJoinedCellInt(
        const Row &row,
        QueryNode *query,
        const string &columnName);

public:
    Executor(Database *database);

    // Executes query and prints result
    void execute(QueryNode *query);
    void executeInsert(InsertNode *node);
    void executeUpdate(UpdateNode *node);
    void executeDelete(DeleteNode *node);
};

#endif