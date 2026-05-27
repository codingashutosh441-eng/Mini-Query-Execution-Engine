#include "parser.h"

string errorMessage;

int pos = 0;

QueryNode *root = nullptr;

bool expect(string val)
{
    if (pos < tokens.size() &&
        tokens[pos].value == val)
    {
        pos++;
        return true;
    }

    return false;
}

QueryNode* parseColumns()
{
    QueryNode* columnsNode =
        createNode("Columns");

    if(pos >= tokens.size())
    {
        errorMessage =
            "Expected column after SELECT";

        return nullptr;
    }

    // SELECT *
    if(tokens[pos].value == "*")
    {
        columnsNode->children.push_back(
            createNode("Column", "*"));

        pos++;

        return columnsNode;
    }

    // first column
    if(tokens[pos].type != "identifier")
    {
        errorMessage =
            "Expected column after SELECT";

        return nullptr;
    }

    columnsNode->children.push_back(
        createNode("Column",
                   tokens[pos].value));

    pos++;

    // additional columns
    while(pos < tokens.size())
    {
        if(tokens[pos].value == ",")
        {
            pos++;

            if(pos >= tokens.size() ||
               tokens[pos].type != "identifier")
            {
                errorMessage =
                    "Expected column name after comma";

                return nullptr;
            }

            columnsNode->children.push_back(
                createNode("Column",
                           tokens[pos].value));

            pos++;
        }

        else if(tokens[pos].value == "FROM")
        {
            break;
        }

        else
        {
            errorMessage =
                "Expected comma or FROM after column name";

            return nullptr;
        }
    }

    return columnsNode;
}

bool parseWhere()
{
    if (pos < tokens.size() &&
        tokens[pos].value == "WHERE")
    {
        pos++;

        // left operand
        if (pos >= tokens.size() ||
            tokens[pos].type != "identifier")
        {
            errorMessage =
                "Expected identifier after WHERE";

            return false;
        }

        string whereLeft =
            tokens[pos].value;

        pos++;

        // operator
        if (pos >= tokens.size() ||
            tokens[pos].type != "operator")
        {
            errorMessage =
                "Expected operator after identifier";

            return false;
        }

        string whereOperator =
            tokens[pos].value;

        pos++;

        // right operand
        if (pos >= tokens.size())
        {
            errorMessage =
                "Expected value after operator";

            return false;
        }

        if (tokens[pos].type != "digit" &&
            tokens[pos].type != "string" &&
            tokens[pos].type != "identifier")
        {
            errorMessage =
                "Expected value after operator";

            return false;
        }

        string whereRight =
            tokens[pos].value;

        pos++;

        // BUILD CONDITION NODE
        QueryNode *conditionNode =
            createNode("Comparison");

        conditionNode->children.push_back(
            createNode("Identifier",
                       whereLeft));

        conditionNode->children.push_back(
            createNode("Operator",
                       whereOperator));

        conditionNode->children.push_back(
            createNode("Value",
                       whereRight));

        root->children.push_back(
            conditionNode);
    }

    return true;
}

bool parseSelect()
{
    // CREATE ROOT QUERY NODE
    root = createNode("Query");

    if (!expect("SELECT"))
    {
        errorMessage =
            "Expected SELECT keyword";

        return false;
    }

    QueryNode* columnsNode =
        parseColumns();

    if(columnsNode == nullptr)
    {
        return false;
    }

    root->children.push_back(
        columnsNode);

    if (!expect("FROM"))
    {
        errorMessage =
            "Expected FROM keyword";

        return false;
    }

    // table name
    if (pos >= tokens.size() ||
        tokens[pos].type != "identifier")
    {
        errorMessage =
            "Expected table name after FROM";

        return false;
    }

    QueryNode *tableNode =
        createNode("Table",
                   tokens[pos].value);

    root->children.push_back(
        tableNode);

    pos++;

    if (!parseWhere())
    {
        return false;
    }

    // optional semicolon
    if (pos < tokens.size() &&
        tokens[pos].value == ";")
    {
        pos++;
    }

    if (pos != tokens.size())
    {
        errorMessage =
            "Unexpected extra tokens";

        return false;
    }

    return true;
}