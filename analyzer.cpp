#include "analyzer.h"

#include <sstream>

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

bool SemanticAnalyzer::validateExpression(
    ExpressionNode* node,
    string tableName)
{
    if(node == nullptr)
    {
        return true;
    }

    if(node->isLogical)
    {
        return validateExpression(
                   node->left,
                   tableName)
               &&
               validateExpression(
                   node->right,
                   tableName);
    }

    if(!columnExists(
            tableName,
            node->column))
    {
        errorMessage =
            "Unknown column in WHERE: " +
            node->column;

        return false;
    }

    return true;
}

bool SemanticAnalyzer::validate(
    QueryNode *query)
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

    if(query->whereExpression != nullptr)
    {
        if(!validateExpression(
                query->whereExpression,
                tableName))
        {
            return false;
        }
    }

    return true;
}