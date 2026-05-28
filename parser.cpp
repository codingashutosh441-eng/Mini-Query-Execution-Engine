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

    ConditionNode* condition =
        new ConditionNode();

    if(pos >= tokens.size() ||
       tokens[pos].type != "identifier")
    {
        errorMessage =
            "Expected identifier in WHERE";

        return false;
    }

    condition->left =
        tokens[pos].value;

    pos++;

    if(pos >= tokens.size() ||
       tokens[pos].type != "operator")
    {
        errorMessage =
            "Expected operator in WHERE";

        return false;
    }

    condition->op =
        tokens[pos].value;

    pos++;

    if(pos >= tokens.size())
    {
        errorMessage =
            "Expected value in WHERE";

        return false;
    }

    if(tokens[pos].type != "identifier" &&
        tokens[pos].type != "digit" &&
        tokens[pos].type != "string")
        {
           errorMessage ="Expected valid value in WHERE";

           return false;
        }

    condition->right =
        tokens[pos].value;

    pos++;

    query->condition = condition;

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