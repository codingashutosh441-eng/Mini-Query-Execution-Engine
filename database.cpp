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
    for (auto &pair : indexes)
    {
        Index &index = pair.second;

        if (index.tableName != tableName)
        {
            continue;
        }

        int columnIndex =
            getColumnIndex(
                tableName,
                index.columnName);

        string value =
            row.values[columnIndex].value;

        int rowPosition =
            static_cast<int>(
                tables[tableName]
                    .rows.size() -
                1);

        index.rowPositions[value]
            .push_back(rowPosition);
    }
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

const unordered_map<string, TableSchema> &
Database::getSchemas() const
{
    return schemas;
}

void Database::addLoadedSchema(
    const TableSchema &schema)
{
    schemas[schema.tableName] =
        schema;
}

void Database::addLoadedTable(
    const Table &table)
{
    tables[table.tableName] =
        table;
}

bool Database::createIndex(
    const string &tableName,
    const string &columnName)
{
    if (!tableExists(tableName))
    {
        return false;
    }

    if (!columnExists(
            tableName,
            columnName))
    {
        return false;
    }

    string key =
        tableName + "." +
        columnName;

    if (indexes.count(key))
    {
       return false;
    }

    Index index;

    index.tableName =
        tableName;

    index.columnName =
        columnName;

    Table *table =
        getTable(tableName);

    int columnIndex =
        getColumnIndex(
            tableName,
            columnName);

    for (size_t rowPos = 0;
         rowPos < table->rows.size();
         rowPos++)
    {
        string value =
            table->rows[rowPos]
                .values[columnIndex]
                .value;

        index.rowPositions[value]
            .push_back(
                static_cast<int>(
                    rowPos));
    }

    indexes[key] = index;

    return true;
}

bool Database::hasIndex(
    const string &tableName,
    const string &columnName)
{
    string key =
        tableName + "." +
        columnName;

    return indexes.count(key);
}

vector<Row> Database::lookupIndex(
    const string &tableName,
    const string &columnName,
    const string &value)
{
    vector<Row> result;

    string key =
        tableName + "." +
        columnName;

    if (!indexes.count(key))
    {
        return result;
    }

    Table *table =
        getTable(tableName);

    auto &index =
        indexes[key];

    if (!index.rowPositions.count(value))
    {
        return result;
    }

    for (int rowPos :
         index.rowPositions[value])
    {
        if (rowPos < 0 ||
            static_cast<size_t>(rowPos) >= table->rows.size())
        {
            continue;
        }
        result.push_back(
            table->rows[rowPos]);
    }

    return result;
}

void Database::rebuildIndexes()
{
    vector<pair<string, string>> indexInfo;

    for (const auto &pair : indexes)
    {
        indexInfo.push_back(
            {pair.second.tableName,
             pair.second.columnName});
    }

    indexes.clear();

    for (const auto &info : indexInfo)
    {
        createIndex(
            info.first,
            info.second);
    }
}