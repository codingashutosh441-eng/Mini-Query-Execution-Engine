#include "parser.h"

Parser::Parser(const vector<Token> tokenStream): tokens(tokenStream)
{}

string Parser::getError()
{
    return errorMessage;
}

bool Parser::match(string expected)
{
    if(pos >= tokens.size())
    {
        return false;
    }

    if(tokens[pos].value == expected)
    {
        pos++;
        return true;
    }

    return false;
}

bool Parser::parseColumns(QueryNode* query)
{
    ColumnNode* columns =
        new ColumnNode();

    if(pos >= tokens.size())
    {
        errorMessage =
            "Expected column name";

        return false;
    }

    if(tokens[pos].value == "*")
    {
        columns->selectAll = true;

        pos++;

        query->columns = columns;

        return true;
    }

    while(pos < tokens.size())
    {
        if(tokens[pos].type != "identifier")
        {
            errorMessage =
                "Expected column name";

            return false;
        }

        columns->columns.push_back(
            tokens[pos].value);

        pos++;

        if(pos < tokens.size() &&
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

bool Parser::parseTable(QueryNode* query)
{
    if(pos >= tokens.size() ||
       tokens[pos].type != "identifier")
    {
        errorMessage =
            "Expected table name";

        return false;
    }

    TableNode* table =
        new TableNode();

    table->tableName =
        tokens[pos].value;

    query->table = table;

    pos++;

    return true;
}


bool Parser::parseWhere(QueryNode* query)
{
    if(pos >= tokens.size())
    {
        return true;
    }

    if(tokens[pos].value != "WHERE")
    {
        return true;
    }

    pos++;

    ExpressionNode* root =
        parseExpression();

    if(root == nullptr)
    {
        return false;
    }

    query->whereExpression =
        root;

    return true;
}



QueryNode* Parser::parseSelect()
{
    QueryNode* query =
        new QueryNode();

    if(!match("SELECT"))
    {
        errorMessage =
            "Expected SELECT";

        return nullptr;
    }

    if(!parseColumns(query))
    {
        freeQuery(query);
        return nullptr;
    }

    if(!match("FROM"))
    {
        errorMessage =
            "Expected FROM";
        freeQuery(query);
        return nullptr;
    }

    if(!parseTable(query))
    {
        freeQuery(query);
        return nullptr;
    }

    if(!parseWhere(query))
    {
        freeQuery(query);
        return nullptr;
    }

    if(pos >= tokens.size() ||
        tokens[pos].value != ";"){
        errorMessage ="Expected semicolon";

        freeQuery(query);

        return nullptr;
    }  
    pos++;

    return query;
}

ExpressionNode* Parser::parseConditionExpression()
{
    if(pos >= tokens.size() ||
       tokens[pos].type != "identifier")
    {
        errorMessage =
            "Expected identifier";

        return nullptr;
    }

    string left =
        tokens[pos].value;

    pos++;

    if(pos >= tokens.size() ||
       tokens[pos].type != "operator")
    {
        errorMessage =
            "Expected operator";

        return nullptr;
    }

    string op =
        tokens[pos].value;

    pos++;

    if(pos >= tokens.size())
    {
        errorMessage =
            "Expected value";

        return nullptr;
    }

    if(tokens[pos].type != "identifier" &&
       tokens[pos].type != "digit" &&
       tokens[pos].type != "string")
    {
        errorMessage =
            "Expected valid value";

        return nullptr;
    }

    string right =
        tokens[pos].value;

    pos++;

    ExpressionNode* node =
        new ExpressionNode();

    node->isLogical = false;

    node->column = left;

    node->op = op;

    node->value = right;

    return node;
}

ExpressionNode* Parser::parsePrimary()
{
    // Parenthesized expression

    if(pos < tokens.size() &&
       tokens[pos].value == "(")
    {
        pos++;

        ExpressionNode* expr =
            parseExpression();

        if(expr == nullptr)
        {
            return nullptr;
        }

        if(pos >= tokens.size() ||
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

ExpressionNode* Parser::parseAndExpression()
{
    ExpressionNode* left =
        parsePrimary();

    if(left == nullptr)
    {
        return nullptr;
    }

    while(pos < tokens.size() &&
          tokens[pos].value == "AND")
    {
        pos++;

        ExpressionNode* right =
            parsePrimary();

        if(right == nullptr)
        {
            return nullptr;
        }

        ExpressionNode* andNode =
            new ExpressionNode();

        andNode->isLogical = true;

        andNode->logicalOp = "AND";

        andNode->left = left;

        andNode->right = right;

        left = andNode;
    }

    return left;
}


ExpressionNode* Parser::parseExpression()
{
    ExpressionNode* left =
        parseAndExpression();

    if(left == nullptr)
    {
        return nullptr;
    }

    while(pos < tokens.size() &&
          tokens[pos].value == "OR")
    {
        pos++;

        ExpressionNode* right =
            parseAndExpression();

        if(right == nullptr)
        {
            return nullptr;
        }

        ExpressionNode* orNode =
            new ExpressionNode();

        orNode->isLogical = true;

        orNode->logicalOp = "OR";

        orNode->left = left;

        orNode->right = right;

        left = orNode;
    }

    return left;
}