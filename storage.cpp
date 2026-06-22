#include "storage.h"

#include <sstream>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

string StorageManager::dataTypeToString(
    DataType type)
{
    switch(type)
    {
        case DataType::INT:
            return "INT";

        case DataType::STRING:
            return "STRING";

        case DataType::FLOAT:
            return "FLOAT";
    }

    return "STRING";
}

bool StorageManager::createTableFiles(
    const TableSchema& schema)
{
    fs::create_directories(
        "database");

    string schemaPath =
        "database/" +
        schema.tableName +
        ".schema";

    string csvPath =
        "database/" +
        schema.tableName +
        ".csv";

    ofstream schemaFile(
        schemaPath);

    if (!schemaFile.is_open())
    {
        return false;
    }

    for (const auto& column :
         schema.columns)
    {
        schemaFile
            << column.name
            << ","
            << dataTypeToString(
                   column.type)
            << "\n";
    }

    schemaFile.close();

    ofstream csvFile(
        csvPath);

    if (!csvFile.is_open())
    {
        return false;
    }

    csvFile.close();

    return true;
}

bool StorageManager::appendRow(
    const string& tableName,
    const Row& row)
{
    string csvPath =
        "database/" +
        tableName +
        ".csv";

    ofstream file(
        csvPath,
        ios::app);

    if (!file.is_open())
    {
        return false;
    }

    for (size_t i = 0;
         i < row.values.size();
         i++)
    {
        file
            << row.values[i].value;

        if (i + 1 <
            row.values.size())
        {
            file << ",";
        }
    }

    file << "\n";

    return true;
}


DataType StorageManager::stringToDataType(
    const string& type)
{
    if (type == "INT")
        return DataType::INT;

    if (type == "FLOAT")
        return DataType::FLOAT;

    return DataType::STRING;
}

void StorageManager::loadDatabase(
    Database& db)
{
    namespace fs = std::filesystem;

    if (!fs::exists("database"))
    {
        return;
    }

    for (const auto& entry :
         fs::directory_iterator("database"))
    {
        if (entry.path().extension() != ".schema")
        {
            continue;
        }

        string schemaPath =
            entry.path().string();

        string tableName =
            entry.path().stem().string();

        // -------------------------
        // LOAD SCHEMA
        // -------------------------

        TableSchema schema;
        schema.tableName = tableName;

        ifstream schemaFile(
            schemaPath);

        string line;

        while (getline(
            schemaFile,
            line))
        {
            if (line.empty())
            {
                continue;
            }

            stringstream ss(line);

            string columnName;
            string typeName;

            getline(
                ss,
                columnName,
                ',');

            getline(
                ss,
                typeName,
                ',');

            ColumnInfo column;

            column.name =
                columnName;

            column.type =
                stringToDataType(
                    typeName);

            schema.columns.push_back(
                column);
        }

        schemaFile.close();

        db.addLoadedSchema(
            schema);

        // -------------------------
        // LOAD TABLE
        // -------------------------

        Table table(
            tableName);

        string csvPath =
            "database/" +
            tableName +
            ".csv";

        ifstream csvFile(
            csvPath);

        while (getline(
            csvFile,
            line))
        {
            if (line.empty())
            {
                continue;
            }

            stringstream ss(line);

            Row row;

            string value;

            size_t columnIndex = 0;

            while (getline(
                ss,
                value,
                ','))
            {
                Cell cell;

                cell.value = value;

                if (columnIndex <
                    schema.columns.size())
                {
                    cell.type =
                        schema.columns[
                            columnIndex]
                            .type;
                }
                else
                {
                    cell.type =
                        DataType::STRING;
                }

                row.values.push_back(
                    cell);

                columnIndex++;
            }

            table.rows.push_back(
                row);
        }

        csvFile.close();

        db.addLoadedTable(
            table);
    }
}

bool StorageManager::saveTable(
    const Table& table)
{
    string csvPath =
        "database/" +
        table.tableName +
        ".csv";

    ofstream file(
        csvPath,
        ios::trunc);

    if (!file.is_open())
    {
        return false;
    }

    for (const auto& row :
         table.rows)
    {
        for (size_t i = 0;
             i < row.values.size();
             i++)
        {
            file
                << row.values[i].value;

            if (i + 1 <
                row.values.size())
            {
                file << ",";
            }
        }

        file << "\n";
    }

    return true;
}

bool StorageManager::saveIndex(
    const string& tableName,
    const string& columnName)
{
    ofstream file(
        "database/indexes.meta",
        ios::app);

    if (!file.is_open())
    {
        return false;
    }

    file
        << tableName
        << ","
        << columnName
        << "\n";

    return true;
}

void StorageManager::loadIndexes(
    Database& db)
{
    ifstream file(
        "database/indexes.meta");

    if (!file.is_open())
    {
        return;
    }

    string line;

    while (getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        stringstream ss(line);

        string tableName;
        string columnName;

        getline(
            ss,
            tableName,
            ',');

        getline(
            ss,
            columnName,
            ',');

        db.createIndex(
            tableName,
            columnName);
    }
}