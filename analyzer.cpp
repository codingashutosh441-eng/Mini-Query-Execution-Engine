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
    if (s.empty())
        return false;

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

SemanticAnalyzer::SemanticAnalyzer(Database *database)
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
                "Column '" +
                node->column + "' is INT\n"
                               "Value " +
                node->value + " cannot be converted to INT";
            return false;
        }
    }
    else if (columnType == DataType::STRING && literalType == DataType::INT)
    {
        errorMessage =
            "Type Mismatch:\n"
            "Column '" +
            node->column + "' is STRING\n"
                           "Literal '" +
            node->value + "' is INT\n"
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

    // 🔹 ORDERBY validation
    if (query->orderBy != nullptr)
    {
        if (!columnExists(tableName, query->orderBy->column))
        {
            errorMessage =
                "Unknown column in ORDER BY: " +
                query->orderBy->column;
            return false;
        }

        // normalize direction just in case
        if (query->orderBy->direction != "ASC" &&
            query->orderBy->direction != "DESC")
        {
            query->orderBy->direction = "ASC";
        }
    }

    if (query->limit != nullptr)
    {
        if (query->limit->count <= 0)
        {
            errorMessage =
                "LIMIT must be greater than 0";

            return false;
        }
    }

    if (query->columns == nullptr)
    {
        errorMessage = "Missing column list";
        return false;
    }

    bool hasColumns =
        !query->columns->columns.empty();

    bool hasAggregates =
        !query->columns->aggregates.empty();

    if (hasColumns && hasAggregates)
    {
        if (query->groupBy == nullptr)
        {
            errorMessage =
                "Mixing columns and aggregates requires GROUP BY";

            return false;
        }

        for (const string &col :
             query->columns->columns)
        {
            bool found = false;

            for (const string &groupCol :
                 query->groupBy->columns)
            {
                if (col == groupCol)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                errorMessage =
                    "Column must appear in GROUP BY: " +
                    col;

                return false;
            }
        }

        if (query->having != nullptr)
        {
            if (query->groupBy == nullptr)
            {
                errorMessage =
                    "HAVING requires GROUP BY";

                return false;
            }

            AggregateNode agg =
                query->having->aggregate;

            if (agg.type != AggregateType::COUNT)
            {
                if (!columnExists(
                        tableName,
                        agg.column))
                {
                    errorMessage =
                        "Unknown column: " +
                        agg.column;

                    return false;
                }

                DataType type =
                    getColumnType(
                        tableName,
                        agg.column);

                if (type != DataType::INT)
                {
                    errorMessage =
                        "Aggregate requires numeric column: " +
                        agg.column;

                    return false;
                }
            }
        }
    }

    if (query->groupBy != nullptr)
    {
        for (const string &col :
             query->groupBy->columns)
        {
            if (!columnExists(tableName, col))
            {
                errorMessage =
                    "Unknown column in GROUP BY: " +
                    col;

                return false;
            }
        }
    }

    for (const auto &agg :
         query->columns->aggregates)
    {
        // COUNT(*) has no column
        if (agg.type == AggregateType::COUNT)
        {
            continue;
        }

        if (!columnExists(
                tableName,
                agg.column))
        {
            errorMessage =
                "Unknown column: " +
                agg.column;

            return false;
        }

        DataType type =
            db->getColumnType(
                tableName,
                agg.column);

        if ((agg.type == AggregateType::SUM ||
             agg.type == AggregateType::AVG ||
             agg.type == AggregateType::MIN ||
             agg.type == AggregateType::MAX) &&
            type != DataType::INT)
        {
            errorMessage =
                "Aggregate requires numeric column: " +
                agg.column;

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

bool SemanticAnalyzer::validateInsert(
    InsertNode *node)
{
    if (!node)
        return false;

    if (!tableExists(node->tableName))
    {
        errorMessage =
            "Unknown table: " +
            node->tableName;

        return false;
    }

    const TableSchema *schema =
        db->getSchema(node->tableName);

    if (schema == nullptr)
    {
        errorMessage =
            "Schema not found";

        return false;
    }

    for (size_t rowIndex = 0;
         rowIndex < node->rows.size();
         rowIndex++)
    {
        const auto &row =
            node->rows[rowIndex];

        // Column count check

        if (row.values.size() !=
            schema->columns.size())
        {
            errorMessage =
                "Expected " +
                to_string(schema->columns.size()) +
                " values but got " +
                to_string(row.values.size());

            return false;
        }

        // Type validation

        for (size_t colIndex = 0;
             colIndex < row.values.size();
             colIndex++)
        {
            DataType expected =
                schema->columns[colIndex].type;

            DataType actual =
                row.values[colIndex].type;

            if (expected != actual)
            {
                errorMessage =
                    "Column '" +
                    schema->columns[colIndex].name +
                    "' expects " +
                    string(expected == DataType::INT
                               ? "INT"
                               : "STRING");

                return false;
            }
        }
    }

    return true;
}

bool SemanticAnalyzer::validateUpdate(
    UpdateNode *node)
{
    if (!tableExists(node->tableName))
    {
        errorMessage =
            "Unknown table: " +
            node->tableName;

        return false;
    }

    if (!columnExists(
            node->tableName,
            node->columnName))
    {
        errorMessage =
            "Unknown column: " +
            node->columnName;

        return false;
    }

    DataType expected =
        getColumnType(
            node->tableName,
            node->columnName);

    if (expected != node->valueType)
    {
        errorMessage =
            "Type mismatch for column: " +
            node->columnName;

        return false;
    }

    if (node->whereExpression)
    {
        return validateExpression(
            node->whereExpression,
            node->tableName);
    }

    return true;
}

bool SemanticAnalyzer::validateDelete(
    DeleteNode *node)
{
    if (!tableExists(node->tableName))
    {
        errorMessage =
            "Unknown table: " +
            node->tableName;

        return false;
    }

    if (node->whereExpression)
    {
        return validateExpression(
            node->whereExpression,
            node->tableName);
    }

    return true;
}