#include "database.h"

// ---------------- Table ----------------

Table::Table(const string &name)
{
    tableName = name;
}

void Table::insert(const Row &row)
{
    rows.push_back(row);
}

// ---------------- Database ----------------

Database::Database()
{
    schemas["students"] =
    {
        "students",
        {
            {"id", DataType::INT},
            {"name", DataType::STRING},
            {"age", DataType::INT}
        }
    };

    schemas["employees"] =
    {
        "employees",
        {
            {"id", DataType::INT},
            {"name", DataType::STRING},
            {"salary", DataType::INT}
        }
    };
}

void Database::createTable(const string &tableName)
{
    if (tables.find(tableName) == tables.end())
    {
        tables[tableName] = Table(tableName);
    }
}

void Database::insertRow(const string &tableName, const Row &row)
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
        return nullptr;

    return &tables[tableName];
}

bool Database::tableExists(const string &tableName)
{
    return schemas.count(tableName);
}

bool Database::columnExists(const string &tableName, const string &columnName)
{
    auto &cols = schemas[tableName].columns;

    for (const auto &c : cols)
    {
        if (c.name == columnName)
            return true;
    }

    return false;
}

DataType Database::getColumnType(const string &tableName, const string &columnName)
{
    auto &cols = schemas[tableName].columns;

    for (const auto &c : cols)
    {
        if (c.name == columnName)
            return c.type;
    }

    return DataType::STRING;
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

bool Database::createSchema(
    const string& tableName,
    const vector<ColumnInfo>& columns)
{
    if (schemas.count(tableName))
    {
        return false;
    }

    TableSchema schema;

    schema.tableName = tableName;
    schema.columns = columns;

    schemas[tableName] = schema;

    tables[tableName] = Table(tableName);

    return true;
}

const TableSchema* Database::getSchema(
    const string& tableName) const
{
    auto it = schemas.find(tableName);

    if (it == schemas.end())
    {
        return nullptr;
    }

    return &(it->second);
}