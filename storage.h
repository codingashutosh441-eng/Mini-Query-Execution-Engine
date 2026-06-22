#ifndef STORAGE_H
#define STORAGE_H

#include "database.h"

class StorageManager
{
public:
    static bool createTableFiles(
        const TableSchema &schema);

    static bool appendRow(
        const string &tableName,
        const Row &row);
    static void loadDatabase(
        Database &db);
    static bool saveTable(
        const Table &table);

    static bool saveIndex(
        const string &tableName,
        const string &columnName);

    static void loadIndexes(
        Database &db);

private:
    static string dataTypeToString(
        DataType type);
    static DataType stringToDataType(
        const string &type);
};

#endif