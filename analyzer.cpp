#include "analyzer.h"

#include <sstream>

DataType SemanticAnalyzer::getColumnType(
    string tableName,
    string columnName)
{
    return db->getColumnType(tableName, columnName);
}

DataType SemanticAnalyzer::getLiteralType(string value)
{
    if (!value.empty() &&
        value.front() == '\'' &&
        value.back() == '\'')
        return DataType::STRING;

    for (char c : value)
    {
        if (!isdigit(c))
            return DataType::STRING;
    }

    return DataType::INT;
}

bool isNumber(const string &s)
{
    if (s.empty()) return false;

    for (char c : s)
    {
        if (!isdigit(c))
            return false;
    }
    return true;
}


bool tryCastToInt(const string &value, int &out)
{
    if (!isNumber(value))
        return false;

    out = stoi(value);
    return true;
}

SemanticAnalyzer::SemanticAnalyzer(Database* database)
{
    this->db = database;
}

bool SemanticAnalyzer::tableExists(string tableName)
{
    return db->tableExists(tableName);
}

bool SemanticAnalyzer::columnExists(
    string tableName,
    string columnName)
{
    return db->columnExists(tableName, columnName);
}

bool SemanticAnalyzer::validateExpression(
    ExpressionNode *node,
    string tableName)
{
    if (!node)
        return true;

    if (node->isLogical)
    {
        return validateExpression(node->left, tableName) &&
               validateExpression(node->right, tableName);
    }

    if (!columnExists(tableName, node->column))
    {
        errorMessage = "Unknown column in WHERE: " + node->column;
        return false;
    }

    DataType columnType =
        getColumnType(tableName, node->column);

    DataType literalType =
        getLiteralType(node->value);

    //  IMPROVED TYPE HANDLING

    if (columnType == DataType::INT && literalType == DataType::STRING)
    {
        int temp;
        if (!tryCastToInt(node->value, temp))
        {
            errorMessage =
                "Invalid INT conversion:\n"
                "Column '" + node->column + "' is INT\n"
                "Value " + node->value + " cannot be converted to INT";
            return false;
        }
    }
    else if (columnType == DataType::STRING && literalType == DataType::INT)
    {
        errorMessage =
            "Type Mismatch:\n"
            "Column '" + node->column + "' is STRING\n"
            "Literal '" + node->value + "' is INT\n"
            "Implicit cast INT → STRING not allowed";
        return false;
    }

    //  OPERATOR VALIDATION (unchanged)
    if (!isOperatorValid(columnType, node->op))
    {
        errorMessage =
            "Operator '" + node->op +
            "' not supported for column: " + node->column;

        return false;
    }

    return true;
}

bool SemanticAnalyzer::validate(QueryNode *query)
{
    if (!query)
        return false;

    string tableName = query->table->tableName;

    if (!tableExists(tableName))
    {
        errorMessage = "Unknown table: " + tableName;
        return false;
    }

    // 🔹 Column validation
    if (query->columns && !query->columns->selectAll)
    {
        for (string col : query->columns->columns)
        {
            if (!columnExists(tableName, col))
            {
                errorMessage = "Unknown column: " + col;
                return false;
            }
        }
    }

    // 🔹 WHERE validation
    if (query->whereExpression)
    {
        if (!validateExpression(query->whereExpression, tableName))
        {
            return false;
        }
    }

    return true;
}
bool SemanticAnalyzer::isOperatorValid(DataType type, string op)
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
        return op == "=" || op == "!=";
    }

    return false;
}