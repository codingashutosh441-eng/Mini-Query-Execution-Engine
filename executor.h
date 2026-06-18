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
    string groupKey;

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

    long long calculateCount(
        const vector<Row> &rows);

    long long calculateSum(
        const vector<Row> &rows,
        const string &tableName,
        const string &column);

    double calculateAvg(
        const vector<Row> &rows,
        const string &tableName,
        const string &column);

    string calculateMin(
        const vector<Row> &rows,
        const string &tableName,
        const string &column);

    string calculateMax(
        const vector<Row> &rows,
        const string &tableName,
        const string &column);

public:
    Executor(Database *database);

    // Executes query and prints result
    void execute(QueryNode *query);
    void executeInsert(InsertNode *node);
    void executeUpdate(UpdateNode *node);
    void executeDelete(DeleteNode *node);
};

#endif