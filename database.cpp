#include "database.h"

// ---------- Table ----------

Table::Table(const string &name)
{
    tableName = name;
}

void Table::insert(const Row &row)
{
    rows.push_back(row);
}

// ---------- Database ----------

void Database::createTable(const string &tableName)
{
    if (tables.find(tableName) == tables.end())
    {
        tables[tableName] = Table(tableName);
    }
}

void Database::insertRow(const string &tableName,
                         const Row &row)
{
    if (tables.find(tableName) == tables.end())
    {
        createTable(tableName);
    }

    tables[tableName].insert(row);
}

Table *Database::getTable(const string &tableName)
{
    if (tables.find(tableName) == tables.end())
    {
        return nullptr;
    }

    return &tables[tableName];
}

void Database::seedStudents()
{
    createTable("students");

    insertRow("students", {1, "Rahul", 20});
    insertRow("students", {2, "Amit", 16});
    insertRow("students", {3, "Neha", 22});
    insertRow("students", {4, "Alex", 28});
    insertRow("students", {5, "Son", 20});
    insertRow("students", {6, "Eren", 40});
}