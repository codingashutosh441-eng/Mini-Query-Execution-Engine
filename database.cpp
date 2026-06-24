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

string Database::getLastError() const
{
    return lastError;
}

void Database::createTable(const string &tableName)
{
    if (tables.find(tableName) == tables.end())
    {
        tables[tableName] = Table(tableName);
    }
}

bool Database::insertRow(const string &tableName, const Row &row)
{
    const TableSchema *schema =
        getSchema(tableName);

    if (!schema)
    {
        return false;
    }
    if (tables.find(tableName) == tables.end())
    {
        createTable(tableName);
    }

    for (size_t col = 0;
         col < schema->columns.size();
         col++)
    {
        const ColumnInfo &column =
            schema->columns[col];

        if (!column.primaryKey &&
            !column.unique)
        {
            continue;
        }

        string newValue =
            row.values[col].value;

        for (const Row &existing :
             tables[tableName].rows)
        {

            if (existing.values[col].value ==
                newValue)
            {
                if (column.primaryKey)
                {
                    lastError =
                        "Duplicate value for PRIMARY KEY column: " +
                        column.name;
                }
                else
                {
                    lastError =
                        "Duplicate value for UNIQUE column: " +
                        column.name;
                }

                return false;
            }
        }
    }

    tables[tableName].insert(row);
    for (auto &pair : indexes)
    {
        Index &index = pair.second;

        if (index.tableName != tableName)
        {
            continue;
        }

        vector<string> values;

        for (const string &column :
             index.columnNames)
        {
            int columnIndex =
                getColumnIndex(
                    tableName,
                    column);

            values.push_back(
                row.values[columnIndex]
                    .value);
        }

        string compositeKey =
            buildCompositeKey(
                values);

        int rowPosition =
            static_cast<int>(
                tables[tableName]
                    .rows.size() -
                1);

        index.rowPositions[compositeKey]
            .push_back(
                rowPosition);
    }

    return true;
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

string Database::buildCompositeKey(
    const vector<string> &values)
{
    string key;

    for (size_t i = 0;
         i < values.size();
         i++)
    {
        key += values[i];

        if (i + 1 < values.size())
        {
            key += "|";
        }
    }

    return key;
}

bool Database::createIndex(
    const string &tableName,
    const vector<string> &columnNames)
{
    if (!tableExists(tableName))
    {
        return false;
    }

    for (const string &column :
         columnNames)
    {
        if (!columnExists(
                tableName,
                column))
        {
            return false;
        }
    }

    string key = tableName;

    for (const string &column :
         columnNames)
    {
        key += "." + column;
    }

    if (indexes.count(key))
    {
        return false;
    }

    Index index;

    index.tableName = tableName;
    index.columnNames = columnNames;

    Table *table =
        getTable(tableName);

    for (size_t rowPos = 0;
         rowPos < table->rows.size();
         rowPos++)
    {
        vector<string> values;

        for (const string &column :
             columnNames)
        {
            int columnIndex =
                getColumnIndex(
                    tableName,
                    column);

            values.push_back(
                table->rows[rowPos]
                    .values[columnIndex]
                    .value);
        }

        string compositeKey =
            buildCompositeKey(values);

        index.rowPositions[compositeKey]
            .push_back(
                static_cast<int>(
                    rowPos));
    }

    indexes[key] = index;

    return true;
}

bool Database::hasIndex(
    const string &tableName,
    const vector<string> &columnNames)
{
    string key = tableName;

    for (const string &column :
         columnNames)
    {
        key += "." + column;
    }

    return indexes.count(key);
}

vector<Row> Database::lookupIndex(
    const string &tableName,
    const vector<string> &columnNames,
    const vector<string> &values)
{
    vector<Row> result;

    string key = tableName;

    for (const string &column :
         columnNames)
    {
        key += "." + column;
    }

    if (!indexes.count(key))
    {
        return result;
    }

    string compositeKey =
        buildCompositeKey(values);

    Table *table =
        getTable(tableName);

    auto &index =
        indexes[key];

    if (!index.rowPositions.count(
            compositeKey))
    {
        return result;
    }

    for (int rowPos :
         index.rowPositions[compositeKey])
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
    vector<
        pair<
            string,
            vector<string>>>
        indexInfo;

    for (const auto &pair :
         indexes)
    {
        indexInfo.push_back(
            {pair.second.tableName,
             pair.second.columnNames});
    }

    indexes.clear();

    for (const auto &info :
         indexInfo)
    {
        createIndex(
            info.first,
            info.second);
    }
}