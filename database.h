#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "ast.h"

using namespace std;

// ONLY schema types here
struct ColumnInfo
{
    string name;
    DataType type;
};

struct TableSchema
{
    string tableName;
    vector<ColumnInfo> columns;
};

struct Row
{
    int id;
    string name;
    int age;
};

class Table
{
public:
    string tableName;
    vector<Row> rows;

    Table() = default;
    Table(const string &name);

    void insert(const Row &row);
};

class Database
{
private:
    unordered_map<string, Table> tables;
    unordered_map<string, TableSchema> schemas;

public:
    Database();

    void createTable(const string &tableName);
    void insertRow(const string &tableName, const Row &row);
    Table *getTable(const string &tableName);

    bool tableExists(const string &tableName);
    bool columnExists(const string &tableName, const string &columnName);
    DataType getColumnType(const string &tableName, const string &columnName);

    bool createSchema(
        const string &tableName,
        const vector<ColumnInfo> &columns);

    const TableSchema *getSchema(
        const string &tableName) const;

    void seedStudents();
};

#endif