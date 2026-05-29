#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

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

public:
    void createTable(const string &tableName);

    void insertRow(const string &tableName,
                   const Row &row);

    Table *getTable(const string &tableName);

    void seedStudents();
};

#endif