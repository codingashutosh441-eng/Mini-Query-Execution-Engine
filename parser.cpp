#include "parser.h"

Parser::Parser(const vector<Token> tokenStream) : tokens(tokenStream)
{
}

string Parser::getError()
{
    return errorMessage;
}

bool Parser::match(string expected)
{
    if (pos >= tokens.size())
    {
        return false;
    }

    if (tokens[pos].value == expected)
    {
        pos++;
        return true;
    }

    return false;
}

bool Parser::parseColumns(
    QueryNode *query)
{
    ColumnNode *columns =
        new ColumnNode();

    if (pos < tokens.size() &&
        tokens[pos].value == "DISTINCT")
    {
        columns->distinct = true;

        pos++;
        cout << "DISTINCT DETECTED" << endl;
    }

    if (pos >= tokens.size())
    {
        errorMessage =
            "Expected column name";

        return false;
    }

    if (tokens[pos].value == "*")
    {
        columns->selectAll = true;

        pos++;

        query->columns = columns;

        return true;
    }

    while (pos < tokens.size())
    {
        if (tokens[pos].value == "COUNT" ||
            tokens[pos].value == "SUM" ||
            tokens[pos].value == "AVG" ||
            tokens[pos].value == "MIN" ||
            tokens[pos].value == "MAX")
        {
            if (!parseAggregate(
                    columns))
            {
                return false;
            }
        }
        else
        {
            if (tokens[pos].type != "identifier")
            {
                errorMessage =
                    "Expected column name";

                return false;
            }

            columns->columns.push_back(
                tokens[pos].value);

            pos++;
        }

        if (pos < tokens.size() &&
            tokens[pos].value == ",")
        {
            pos++;
        }
        else
        {
            break;
        }
    }

    query->columns = columns;

    return true;
}

bool Parser::parseTable(QueryNode *query)
{
    if (pos >= tokens.size() ||
        tokens[pos].type != "identifier")
    {
        errorMessage = "Expected table name";
        return false;
    }

    TableNode *table = new TableNode();
    table->tableName = tokens[pos].value;

    query->table = table;
    pos++;

    return true;
}

bool Parser::parseWhere(QueryNode *query)
{
    if (pos >= tokens.size())
        return true;

    if (tokens[pos].value != "WHERE")
        return true;

    pos++;

    ExpressionNode *root = parseExpression();

    if (!root)
        return false;

    query->whereExpression = root;
    return true;
}

bool Parser::parseOrderBy(QueryNode *query)
{
    if (pos >= tokens.size())
        return true;

    if (tokens[pos].value != "ORDER")
        return true;

    pos++;

    if (pos >= tokens.size() || tokens[pos].value != "BY")
    {
        errorMessage = "Expected BY after ORDER";
        return false;
    }

    pos++;

    if (pos >= tokens.size() || tokens[pos].type != "identifier")
    {
        errorMessage = "Expected column name in ORDER BY";
        return false;
    }

    OrderByNode *node = new OrderByNode();
    node->column = tokens[pos].value;
    pos++;

    // default
    node->direction = "ASC";

    if (pos < tokens.size())
    {
        if (tokens[pos].value == "ASC" || tokens[pos].value == "DESC")
        {
            node->direction = tokens[pos].value;
            pos++;
        }
    }

    query->orderBy = node;
    return true;
}

bool Parser::parseLimit(QueryNode *query)
{
    if (pos >= tokens.size())
    {
        return true;
    }

    if (tokens[pos].value != "LIMIT")
    {
        return true;
    }

    pos++;

    if (pos >= tokens.size() ||
        tokens[pos].type != "digit")
    {
        errorMessage =
            "Expected number after LIMIT";

        return false;
    }

    LimitNode *node =
        new LimitNode();

    node->count =
        stoi(tokens[pos].value);

    pos++;

    query->limit = node;

    return true;
}

bool Parser::parseAggregate(
    ColumnNode *columns)
{
    AggregateNode agg;

    string func =
        tokens[pos].value;

    if (func == "COUNT")
        agg.type = AggregateType::COUNT;

    else if (func == "SUM")
        agg.type = AggregateType::SUM;

    else if (func == "AVG")
        agg.type = AggregateType::AVG;

    else if (func == "MIN")
        agg.type = AggregateType::MIN;

    else if (func == "MAX")
        agg.type = AggregateType::MAX;

    else
        return false;

    pos++;

    if (!match("("))
    {
        errorMessage =
            "Expected '('";

        return false;
    }

    if (agg.type ==
        AggregateType::COUNT)
    {
        if (pos < tokens.size() &&
            tokens[pos].value == "*")
        {
            agg.countStar = true;

            pos++;
        }
        else
        {
            errorMessage =
                "Expected * inside COUNT";

            return false;
        }
    }
    else
    {
        if (pos >= tokens.size() ||
            tokens[pos].type != "identifier")
        {
            errorMessage =
                "Expected column name";

            return false;
        }

        agg.column =
            tokens[pos].value;

        pos++;
    }

    if (!match(")"))
    {
        errorMessage =
            "Expected ')'";

        return false;
    }

    columns->aggregates.push_back(
        agg);

    return true;
}

bool Parser::parseGroupBy(QueryNode *query)
{
    if (pos >= tokens.size())
        return true;

    if (tokens[pos].value != "GROUP")
        return true;

    pos++;

    if (pos >= tokens.size() ||
        tokens[pos].value != "BY")
    {
        errorMessage =
            "Expected BY after GROUP";

        return false;
    }

    pos++;

    GroupByNode *node =
        new GroupByNode();

    while (pos < tokens.size())
    {
        if (tokens[pos].type != "identifier")
        {
            errorMessage =
                "Expected column name";

            delete node;
            return false;
        }

        node->columns.push_back(
            tokens[pos].value);

        pos++;

        if (pos < tokens.size() &&
            tokens[pos].value == ",")
        {
            pos++;
        }
        else
        {
            break;
        }
    }

    query->groupBy = node;

    return true;
}

bool Parser::parseHaving(QueryNode *query)
{
    if (pos >= tokens.size())
        return true;

    if (tokens[pos].value != "HAVING")
        return true;

    pos++;

    HavingNode *node =
        new HavingNode();

    // -------------------------
    // Aggregate Function
    // -------------------------

    if (pos >= tokens.size())
    {
        errorMessage =
            "Expected aggregate function";

        delete node;
        return false;
    }

    string func =
        tokens[pos].value;

    if (func == "COUNT")
        node->aggregate.type = AggregateType::COUNT;

    else if (func == "SUM")
        node->aggregate.type = AggregateType::SUM;

    else if (func == "AVG")
        node->aggregate.type = AggregateType::AVG;

    else if (func == "MIN")
        node->aggregate.type = AggregateType::MIN;

    else if (func == "MAX")
        node->aggregate.type = AggregateType::MAX;

    else
    {
        errorMessage =
            "Expected aggregate function";

        delete node;
        return false;
    }

    pos++;

    if (!match("("))
    {
        errorMessage =
            "Expected '('";

        delete node;
        return false;
    }

    if (node->aggregate.type ==
        AggregateType::COUNT)
    {
        if (pos < tokens.size() &&
            tokens[pos].value == "*")
        {
            node->aggregate.countStar = true;
            pos++;
        }
        else
        {
            errorMessage =
                "Expected * inside COUNT";

            delete node;
            return false;
        }
    }
    else
    {
        if (pos >= tokens.size() ||
            tokens[pos].type != "identifier")
        {
            errorMessage =
                "Expected column name";

            delete node;
            return false;
        }

        node->aggregate.column =
            tokens[pos].value;

        pos++;
    }

    if (!match(")"))
    {
        errorMessage =
            "Expected ')'";

        delete node;
        return false;
    }

    // -------------------------
    // Operator
    // -------------------------

    if (pos >= tokens.size() ||
        tokens[pos].type != "operator")
    {
        errorMessage =
            "Expected operator";

        delete node;
        return false;
    }

    node->op =
        tokens[pos].value;

    pos++;

    // -------------------------
    // Value
    // -------------------------

    if (pos >= tokens.size() ||
        tokens[pos].type != "digit")
    {
        errorMessage =
            "Expected numeric value";

        delete node;
        return false;
    }

    node->value =
        tokens[pos].value;

    pos++;

    query->having = node;

    return true;
}

CreateIndexNode *Parser::parseCreateIndex()
{
    CreateIndexNode *node =
        new CreateIndexNode();

    if (!match("CREATE"))
    {
        errorMessage =
            "Expected CREATE";

        delete node;
        return nullptr;
    }

    if (!match("INDEX"))
    {
        errorMessage =
            "Expected INDEX";

        delete node;
        return nullptr;
    }

    if (tokens[pos].type != "identifier")
    {
        errorMessage =
            "Expected index name";

        delete node;
        return nullptr;
    }

    node->indexName =
        tokens[pos].value;

    pos++;

    if (!match("ON"))
    {
        errorMessage =
            "Expected ON";

        delete node;
        return nullptr;
    }

    if (tokens[pos].type != "identifier")
    {
        errorMessage =
            "Expected table name";

        delete node;
        return nullptr;
    }

    node->tableName =
        tokens[pos].value;

    pos++;

    if (!match("("))
    {
        errorMessage =
            "Expected (";

        delete node;
        return nullptr;
    }

    if (tokens[pos].type != "identifier")
    {
        errorMessage =
            "Expected column name";

        delete node;
        return nullptr;
    }

    node->columnName =
        tokens[pos].value;

    pos++;

    if (!match(")"))
    {
        errorMessage =
            "Expected )";

        delete node;
        return nullptr;
    }

    return node;
}

QueryNode *Parser::parseSelect()
{
    QueryNode *query = new QueryNode();

    if (match("EXPLAIN"))
    {
        query->explainMode = true;
    }

    if (!match("SELECT"))
    {
        errorMessage = "Expected SELECT";
        delete query;
        return nullptr;
    }

    if (!parseColumns(query))
    {
        freeQuery(query);
        return nullptr;
    }

    if (!match("FROM"))
    {
        errorMessage = "Expected FROM";
        freeQuery(query);
        return nullptr;
    }

    if (!parseTable(query))
    {
        freeQuery(query);
        return nullptr;
    }
    if (!parseJoins(query))
    {
        freeQuery(query);
        return nullptr;
    }

    if (!parseWhere(query))
    {
        freeQuery(query);
        return nullptr;
    }

    if (!parseGroupBy(query))
    {
        freeQuery(query);
        return nullptr;
    }

    if (!parseHaving(query))
    {
        freeQuery(query);
        return nullptr;
    }

    if (!parseOrderBy(query))
    {
        freeQuery(query);
        return nullptr;
    }

    if (!parseLimit(query))
    {
        freeQuery(query);
        return nullptr;
    }

    if (pos >= tokens.size() ||
        tokens[pos].value != ";")
    {
        errorMessage = "Expected semicolon";
        freeQuery(query);
        return nullptr;
    }

    pos++;

    return query;
}

ExpressionNode *Parser::parseConditionExpression()
{
    if (pos >= tokens.size() ||
        tokens[pos].type != "identifier")
    {
        errorMessage =
            "Expected identifier";

        return nullptr;
    }

    string left =
        tokens[pos].value;

    pos++;

    if (pos >= tokens.size() ||
        tokens[pos].type != "operator")
    {
        errorMessage =
            "Expected operator";

        return nullptr;
    }

    string op =
        tokens[pos].value;

    pos++;

    if (pos >= tokens.size())
    {
        errorMessage =
            "Expected value";

        return nullptr;
    }

    if (tokens[pos].type != "identifier" &&
        tokens[pos].type != "digit" &&
        tokens[pos].type != "string")
    {
        errorMessage =
            "Expected valid value";

        return nullptr;
    }

    string literalTokenType =
        tokens[pos].type;

    string right =
        tokens[pos].value;

    pos++;

    ExpressionNode *node =
        new ExpressionNode();

    if (literalTokenType == "digit")
    {
        node->valueType =
            DataType::INT;
    }
    else if (literalTokenType == "string")
    {
        node->valueType = DataType::STRING;
    }
    else
    {
        node->valueType =
            DataType::STRING;
    }

    node->isLogical = false;

    node->column = left;

    node->op = op;

    node->value = right;

    return node;
}

ExpressionNode *Parser::parsePrimary()
{
    // Parenthesized expression

    if (pos < tokens.size() &&
        tokens[pos].value == "(")
    {
        pos++;

        ExpressionNode *expr =
            parseExpression();

        if (expr == nullptr)
        {
            return nullptr;
        }

        if (pos >= tokens.size() ||
            tokens[pos].value != ")")
        {
            errorMessage =
                "Expected ')'";

            return nullptr;
        }

        pos++;

        return expr;
    }

    // Normal condition

    return parseConditionExpression();
}

ExpressionNode *Parser::parseAndExpression()
{
    ExpressionNode *left =
        parsePrimary();

    if (left == nullptr)
    {
        return nullptr;
    }

    while (pos < tokens.size() &&
           tokens[pos].value == "AND")
    {
        pos++;

        ExpressionNode *right =
            parsePrimary();

        if (right == nullptr)
        {
            return nullptr;
        }

        ExpressionNode *andNode =
            new ExpressionNode();

        andNode->isLogical = true;

        andNode->logicalOp = "AND";

        andNode->left = left;

        andNode->right = right;

        left = andNode;
    }

    return left;
}

ExpressionNode *Parser::parseExpression()
{
    ExpressionNode *left =
        parseAndExpression();

    if (left == nullptr)
    {
        return nullptr;
    }

    while (pos < tokens.size() &&
           tokens[pos].value == "OR")
    {
        pos++;

        ExpressionNode *right =
            parseAndExpression();

        if (right == nullptr)
        {
            return nullptr;
        }

        ExpressionNode *orNode =
            new ExpressionNode();

        orNode->isLogical = true;

        orNode->logicalOp = "OR";

        orNode->left = left;

        orNode->right = right;

        left = orNode;
    }

    return left;
}

CreateTableNode *Parser::parseCreateTable()
{
    if (!match("CREATE"))
    {
        errorMessage = "Expected CREATE";
        return nullptr;
    }

    if (!match("TABLE"))
    {
        errorMessage = "Expected TABLE";
        return nullptr;
    }

    CreateTableNode *node =
        new CreateTableNode();

    if (pos >= tokens.size() ||
        tokens[pos].type != "identifier")
    {
        errorMessage = "Expected table name";
        delete node;
        return nullptr;
    }

    node->tableName = tokens[pos].value;
    pos++;

    if (!match("("))
    {
        errorMessage = "Expected '('";
        delete node;
        return nullptr;
    }

    // Empty table check
    if (pos < tokens.size() &&
        tokens[pos].value == ")")
    {
        errorMessage =
            "Table must contain at least one column";

        delete node;
        return nullptr;
    }

    unordered_set<string> seenColumns;

    while (true)
    {
        ColumnDefinitionNode column;

        if (tokens[pos].value == ")")
        {
            errorMessage = "Trailing comma before ')'";
            delete node;
            return nullptr;
        }

        if (pos >= tokens.size() ||
            tokens[pos].type != "identifier")
        {
            errorMessage = "Expected column name";
            delete node;
            return nullptr;
        }

        column.name = tokens[pos].value;
        pos++;

        if (seenColumns.count(column.name))
        {
            errorMessage =
                "Duplicate column name: " + column.name;

            delete node;
            return nullptr;
        }

        seenColumns.insert(column.name);

        if (pos >= tokens.size() ||
            tokens[pos].type == "rparen" ||
            tokens[pos].value == ")" ||
            tokens[pos].value == ";")
        {
            errorMessage = "Expected data type";
            delete node;
            return nullptr;
        }

        if (tokens[pos].value == "INT")
        {
            column.type = DataType::INT;
        }
        else if (tokens[pos].value == "STRING")
        {
            column.type = DataType::STRING;
        }
        else if (tokens[pos].value == "FLOAT")
        {
            column.type = DataType::FLOAT;
        }
        else
        {
            errorMessage = "Unsupported data type";
            delete node;
            return nullptr;
        }

        pos++;

        while (pos < tokens.size())
        {
            if (tokens[pos].value == "PRIMARY")
            {
                pos++;

                if (pos >= tokens.size() ||
                    tokens[pos].value != "KEY")
                {
                    errorMessage =
                        "Expected KEY after PRIMARY";

                    delete node;
                    return nullptr;
                }

                column.isPrimaryKey = true;

                pos++;
            }
            else if (tokens[pos].value == "UNIQUE")
            {
                column.isUnique = true;

                pos++;
            }
            else
            {
                break;
            }
        }

        node->columns.push_back(column);

        if (pos >= tokens.size())
        {
            errorMessage = "Expected ',' or ')'";
            delete node;
            return nullptr;
        }

        if (tokens[pos].value == ",")
        {
            pos++;
            continue;
        }

        if (tokens[pos].value == ")")
        {
            break;
        }

        errorMessage = "Expected ',' or ')'";
        delete node;
        return nullptr;
    }

    if (!match(")"))
    {
        errorMessage = "Expected ')'";
        delete node;
        return nullptr;
    }

    if (!match(";"))
    {
        errorMessage = "Expected ';'";
        delete node;
        return nullptr;
    }

    return node;
}

InsertNode *Parser::parseInsert()
{
    if (!match("INSERT"))
    {
        errorMessage = "Expected INSERT";
        return nullptr;
    }

    if (!match("INTO"))
    {
        errorMessage = "Expected INTO";
        return nullptr;
    }

    InsertNode *node = new InsertNode();

    if (pos >= tokens.size() ||
        tokens[pos].type != "identifier")
    {
        errorMessage = "Expected table name";
        delete node;
        return nullptr;
    }

    node->tableName = tokens[pos].value;
    pos++;

    if (!match("VALUES"))
    {
        errorMessage = "Expected VALUES";
        delete node;
        return nullptr;
    }

    while (true)
    {
        if (!match("("))
        {
            errorMessage = "Expected '('";
            delete node;
            return nullptr;
        }

        InsertRowNode row;

        while (true)
        {
            if (pos >= tokens.size())
            {
                errorMessage = "Unexpected end of query";
                delete node;
                return nullptr;
            }

            InsertValueNode value;

            if (tokens[pos].type == "digit")
            {
                value.value = tokens[pos].value;
                value.type = DataType::INT;
            }
            else if (tokens[pos].type == "string")
            {
                value.value = tokens[pos].value;
                value.type = DataType::STRING;
            }
            else
            {
                errorMessage = "Expected value";
                delete node;
                return nullptr;
            }

            row.values.push_back(value);

            pos++;

            if (pos < tokens.size() &&
                tokens[pos].value == ",")
            {
                pos++;
                continue;
            }

            break;
        }

        if (!match(")"))
        {
            errorMessage = "Expected ')'";
            delete node;
            return nullptr;
        }

        node->rows.push_back(row);

        if (pos < tokens.size() &&
            tokens[pos].value == ",")
        {
            pos++;
            continue;
        }

        break;
    }

    if (!match(";"))
    {
        errorMessage = "Expected ';'";
        delete node;
        return nullptr;
    }

    return node;
}

UpdateNode *Parser::parseUpdate()
{
    if (!match("UPDATE"))
    {
        errorMessage = "Expected UPDATE";
        return nullptr;
    }

    UpdateNode *node = new UpdateNode();

    if (pos >= tokens.size() ||
        tokens[pos].type != "identifier")
    {
        errorMessage = "Expected table name";
        delete node;
        return nullptr;
    }

    node->tableName = tokens[pos].value;
    pos++;

    if (!match("SET"))
    {
        errorMessage = "Expected SET";
        delete node;
        return nullptr;
    }

    if (pos >= tokens.size() ||
        tokens[pos].type != "identifier")
    {
        errorMessage = "Expected column name";
        delete node;
        return nullptr;
    }

    node->columnName = tokens[pos].value;
    pos++;

    if (!match("="))
    {
        errorMessage = "Expected =";
        delete node;
        return nullptr;
    }

    if (pos >= tokens.size())
    {
        errorMessage = "Expected value";
        delete node;
        return nullptr;
    }

    node->newValue = tokens[pos].value;

    if (tokens[pos].type == "digit")
    {
        node->valueType = DataType::INT;
    }
    else if (tokens[pos].type == "string")
    {
        node->valueType = DataType::STRING;
    }
    else
    {
        errorMessage = "Invalid value";
        delete node;
        return nullptr;
    }

    pos++;

    if (pos < tokens.size() &&
        tokens[pos].value == "WHERE")
    {
        pos++;

        node->whereExpression =
            parseExpression();

        if (!node->whereExpression)
        {
            errorMessage =
                "Invalid WHERE clause";

            delete node;
            return nullptr;
        }
    }

    if (!match(";"))
    {
        errorMessage = "Expected ';'";
        delete node;
        return nullptr;
    }

    return node;
}

DeleteNode *Parser::parseDelete()
{
    DeleteNode *node =
        new DeleteNode();

    if (!match("DELETE"))
    {
        errorMessage =
            "Expected DELETE";

        delete node;
        return nullptr;
    }

    if (!match("FROM"))
    {
        errorMessage =
            "Expected FROM";

        delete node;
        return nullptr;
    }

    if (pos >= tokens.size() ||
        tokens[pos].type != "identifier")
    {
        errorMessage =
            "Expected table name";

        delete node;
        return nullptr;
    }

    node->tableName =
        tokens[pos].value;

    pos++;

    if (pos < tokens.size() &&
        tokens[pos].value == "WHERE")
    {
        pos++;

        node->whereExpression =
            parseExpression();

        if (!node->whereExpression)
        {
            errorMessage =
                "Invalid WHERE clause";

            delete node;
            return nullptr;
        }
    }

    return node;
}

bool Parser::parseJoins(QueryNode *query)
{
    while (pos < tokens.size() &&
           (tokens[pos].value == "INNER" ||
            tokens[pos].value == "LEFT" ||
            tokens[pos].value == "RIGHT"))
    {
        JoinNode join;

        if (tokens[pos].value == "INNER")
        {
            join.type = JoinType::INNER;
        }
        else if (tokens[pos].value == "LEFT")
        {
            join.type = JoinType::LEFT;
        }
        else if (tokens[pos].value == "RIGHT")
        {
            join.type = JoinType::RIGHT;
        }

        pos++;

        if (pos >= tokens.size() ||
            tokens[pos].value != "JOIN")
        {
            errorMessage =
                "Expected JOIN";

            return false;
        }

        pos++;

        if (pos >= tokens.size() ||
            tokens[pos].type != "identifier")
        {
            errorMessage =
                "Expected table name";

            return false;
        }

        join.rightTable =
            tokens[pos].value;

        pos++;

        if (pos >= tokens.size() ||
            tokens[pos].value != "ON")
        {
            errorMessage =
                "Expected ON";

            return false;
        }

        pos++;

        if (pos >= tokens.size() ||
            tokens[pos].type != "identifier")
        {
            errorMessage =
                "Expected left column";

            return false;
        }

        join.leftColumn =
            tokens[pos].value;

        pos++;

        if (pos >= tokens.size() ||
            tokens[pos].value != "=")
        {
            errorMessage =
                "Expected =";

            return false;
        }

        pos++;

        if (pos >= tokens.size() ||
            tokens[pos].type != "identifier")
        {
            errorMessage =
                "Expected right column";

            return false;
        }

        join.rightColumn =
            tokens[pos].value;

        pos++;

        query->joins.push_back(join);
    }

    return true;
}