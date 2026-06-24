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

    bool primaryKey = false;
    bool unique = false;
};

struct TableSchema
{
    string tableName;
    vector<ColumnInfo> columns;
};

struct Cell
{
    string value;
    DataType type;
};

struct Row
{
    vector<Cell> values;
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

struct Index
{
    string tableName;
    vector<string> columnNames;

    unordered_map<
        string,
        vector<int>>
        rowPositions;
};

class Database
{
private:
    unordered_map<string, Table> tables;
    unordered_map<string, TableSchema> schemas;
    unordered_map<string, Index> indexes;
    string lastError;

public:
    Database();

    void createTable(const string &tableName);
    bool insertRow(const string &tableName, const Row &row);
    Table *getTable(const string &tableName);

    bool tableExists(const string &tableName);
    bool columnExists(const string &tableName, const string &columnName);
    DataType getColumnType(const string &tableName, const string &columnName);

    bool createSchema(
        const string &tableName,
        const vector<ColumnInfo> &columns);

    bool createIndex(
        const string &tableName,
         const vector<string> &columnNames);

    bool hasIndex(
        const string &tableName,
         const vector<string> &columnNames);

    vector<Row> lookupIndex(
        const string &tableName,
        const vector<string> &columnNames,
        const vector<string> &values);
    
    string buildCompositeKey(
    const vector<string> &values);

    void rebuildIndexes();

    const TableSchema *getSchema(
        const string &tableName) const;

    void seedStudents();

    int getColumnIndex(
        const string &tableName,
        const string &columnName);

    const unordered_map<string, TableSchema> &
    getSchemas() const;

    void addLoadedSchema(
        const TableSchema &schema);

    void addLoadedTable(
        const Table &table);

    string getLastError() const;
};

#endif