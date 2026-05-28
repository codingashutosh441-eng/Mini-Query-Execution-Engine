#include "analyzer.h"

SemanticAnalyzer::SemanticAnalyzer()
{
    schema["students"] =
    {
        "id",
        "name",
        "age"
    };

    schema["employees"] =
    {
        "id",
        "name",
        "salary"
    };
}

bool SemanticAnalyzer::tableExists(
    string tableName)
{
    return schema.count(tableName);
}

bool SemanticAnalyzer::columnExists(
    string tableName,
    string columnName)
{
    vector<string> columns =
        schema[tableName];

    for(string column : columns)
    {
        if(column == columnName)
        {
            return true;
        }
    }

    return false;
}

bool SemanticAnalyzer::validate(
    QueryNode* query)
{
    if(query == nullptr)
    {
        return false;
    }

    string tableName =
        query->table->tableName;

    if(!tableExists(tableName))
    {
        errorMessage =
            "Unknown table: " +
            tableName;

        return false;
    }

    if(query->columns != nullptr)
    {
        if(!query->columns->selectAll)
        {
            for(string column :
                query->columns->columns)
            {
                if(!columnExists(
                    tableName,
                    column))
                {
                    errorMessage =
                        "Unknown column: " +
                        column;

                    return false;
                }
            }
        }
    }

    if(query->condition != nullptr)
    {
        if(!columnExists(
            tableName,
            query->condition->left))
        {
            errorMessage =
                "Unknown column in WHERE: " +
                query->condition->left;

            return false;
        }
    }

    return true;
}