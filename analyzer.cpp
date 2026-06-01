#include "analyzer.h"

#include <sstream>

DataType SemanticAnalyzer::getColumnType(
    string tableName,
    string columnName)
{
    for (auto column : schema[tableName])
    {
        if (column.name == columnName)
        {
            return column.type;
        }
    }

    return DataType::STRING;
}

SemanticAnalyzer::SemanticAnalyzer()
{
    schema["students"] =
        {
            {"id", DataType::INT},
            {"name", DataType::STRING},
            {"age", DataType::INT}};

    schema["employees"] =
        {
            {"id", DataType::INT},
            {"name", DataType::STRING},
            {"salary", DataType::INT}};
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
    vector<ColumnInfo> columns =
        schema[tableName];

    for (const ColumnInfo &column : columns)
    {
        if (column.name == columnName)
        {
            return true;
        }
    }

    return false;
}

bool SemanticAnalyzer::validateExpression(
    ExpressionNode *node,
    string tableName)
{
    if (node == nullptr)
    {
        return true;
    }

    if (node->isLogical)
    {
        return validateExpression(
                   node->left,
                   tableName) &&
               validateExpression(
                   node->right,
                   tableName);
    }

    if (!columnExists(
            tableName,
            node->column))
    {
        errorMessage =
            "Unknown column in WHERE: " +
            node->column;

        return false;
    }
    DataType columnType =
        getColumnType(
            tableName,
            node->column);

    if (columnType != node->valueType)
    {
        errorMessage =
            "Type mismatch in WHERE clause: " +
            node->column;

        return false;
    }
    // cout << "Checking operator: "<< node->op << endl;

    if (node->op == "==")
    {
        errorMessage =
            "SQL uses '=' instead of '=='";

        return false;
    }

    if (!isOperatorValid(
            columnType,
            node->op))
    {
        errorMessage =
            "Operator '" +
            node->op +
            "' not supported for column: " +
            node->column;

        return false;
    }

    return true;
}

bool SemanticAnalyzer::validate(
    QueryNode *query)
{
    if (query == nullptr)
    {
        return false;
    }

    string tableName =
        query->table->tableName;

    if (!tableExists(tableName))
    {
        errorMessage =
            "Unknown table: " +
            tableName;

        return false;
    }

    if (query->columns != nullptr)
    {
        if (!query->columns->selectAll)
        {
            for (string column :
                 query->columns->columns)
            {
                if (!columnExists(
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

    if (query->whereExpression != nullptr)
    {
        if (!validateExpression(
                query->whereExpression,
                tableName))
        {
            return false;
        }
    }

    return true;
}

bool SemanticAnalyzer::isOperatorValid(
    DataType type,
    string op)
{
    if (type == DataType::INT)
    {
        return op == "=" ||
               op == "!=" ||
               op == ">" ||
               op == "<" ||
               op == ">=" ||
               op == "<=";
    }

    if (type == DataType::STRING)
    {
        return op == "=" ||
               op == "!=";
    }

    return false;
}