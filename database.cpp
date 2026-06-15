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
    if (!schemas.count("students"))
    {
        createSchema(
            "students",
            {{"id", DataType::INT},
             {"name", DataType::STRING},
             {"age", DataType::INT}});
    }
    createTable("students");

    insertRow(
        "students",
        {{{"1", DataType::INT},
          {"Rahul", DataType::STRING},
          {"20", DataType::INT}}});
    insertRow(
        "students",
        {{{"2", DataType::INT},
          {"Alex", DataType::STRING},
          {"24", DataType::INT}}});
    insertRow(
        "students",
        {{{"3", DataType::INT},
          {"Eren", DataType::STRING},
          {"19", DataType::INT}}});
}

bool Database::createSchema(
    const string &tableName,
    const vector<ColumnInfo> &columns)
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

int Database::getColumnIndex(
    const string &tableName,
    const string &columnName)
{
    const TableSchema *schema =
        getSchema(tableName);

    if (!schema)
        return -1;

    for (size_t i = 0;
         i < schema->columns.size();
         i++)
    {
        if (schema->columns[i].name ==
            columnName)
        {
            return static_cast<int>(i);
        }
    }

    return -1;
}

const TableSchema *Database::getSchema(
    const string &tableName) const
{
    auto it = schemas.find(tableName);

    if (it == schemas.end())
    {
        return nullptr;
    }

    return &(it->second);
}