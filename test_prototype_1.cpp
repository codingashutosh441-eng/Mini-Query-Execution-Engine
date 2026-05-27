#include <iostream>
#include <algorithm>
#include <sstream>
#include <set>
#include <vector>
#include <cctype>

using namespace std;

struct Token
{
    string value;
    string type;
};

struct QueryNode
{
    string type;
    string value;

    vector<QueryNode *> children;
};

vector<Token> tokens;

vector<string> selectedColumns;

string tableName;

string whereLeft;
string whereOperator;
string whereRight;

string errorMessage;

int pos = 0;

bool selectAllColumns = false;

QueryNode *root = nullptr;

set<string> mysqlKeywords = {

    "SELECT", "FROM", "WHERE",
    "INSERT", "UPDATE", "DELETE",
    "ORDER", "BY", "INTO",
    "VALUES", "SET"};

set<string> ops = {

    "+", "-", "*", "/",
    "=", "==", "!=", "<", ">",
    "<=", ">=", "%"};



// FUNCTION DECLARATIONS
QueryNode *createNode(string type, string value = "");

void printTree(QueryNode *node, int depth = 0);



bool isNumber(string s)
{
    if (s.empty())
        return false;

    for (char ch : s)
    {
        if (!isdigit(ch))
            return false;
    }

    return true;
}

bool isIdentifier(string s)
{
    if (s.empty())
        return false;

    if (!(isalpha(s[0]) || s[0] == '_'))
        return false;

    for (int i = 1; i < s.length(); i++)
    {
        if (!(isalnum(s[i]) || s[i] == '_'))
            return false;
    }

    return true;
}

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

bool parseColumns()
{
    if (pos >= tokens.size())
    {
        errorMessage = "Expected column after SELECT";
        return false;
    }

    // SELECT *
    if (tokens[pos].value == "*")
    {
        selectAllColumns = true;
        selectedColumns.push_back("*");

        pos++;
        return true;
    }

    // first column
    if (tokens[pos].type != "identifier")
    {
        errorMessage = "Expected column after SELECT";
        return false;
    }

    selectedColumns.push_back(tokens[pos].value);

    pos++;

    // additional columns
    while (pos < tokens.size())
    {
        // comma means next column
        if (tokens[pos].value == ",")
        {
            pos++;

            if (pos >= tokens.size() ||
                tokens[pos].type != "identifier")
            {
                errorMessage = "Expected column name after comma";

                return false;
            }

            selectedColumns.push_back(tokens[pos].value);

            pos++;
        }

        // FROM means end of column list
        else if (tokens[pos].value == "FROM")
        {
            break;
        }

        // invalid token after column
        else
        {
            errorMessage = "Expected comma or FROM after column name";

            return false;
        }
    }

    return true;
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
            errorMessage = "Expected identifier after WHERE";

            return false;
        }

        whereLeft = tokens[pos].value;

        pos++;

        // operator
        if (pos >= tokens.size() ||
            tokens[pos].type != "operator")
        {
            errorMessage = "Expected operator after identifier";

            return false;
        }

        whereOperator = tokens[pos].value;

        pos++;

        // right operand
        if (pos >= tokens.size())
        {
            errorMessage = "Expected value after operator";

            return false;
        }

        if (tokens[pos].type != "digit" &&
            tokens[pos].type != "string" &&
            tokens[pos].type != "identifier")
        {
            errorMessage = "Expected value after operator";

            return false;
        }

        whereRight = tokens[pos].value;

        pos++;
    }

    return true;
}

bool parseSelect()
{
    // CREATE ROOT QUERY NODE
    root = createNode("Query");

    if (!expect("SELECT"))
    {
        errorMessage = "Expected SELECT keyword";

        return false;
    }

    if (!parseColumns())
    {
        return false;
    }

    // BUILD COLUMNS NODE
    QueryNode *columnsNode = createNode("Columns");

    for (string column : selectedColumns)
    {
        columnsNode->children.push_back(
            createNode("Column", column));
    }

    root->children.push_back(columnsNode);

    if (!expect("FROM"))
    {
        errorMessage = "Expected FROM keyword";

        return false;
    }

    // table name
    if (pos >= tokens.size() ||
        tokens[pos].type != "identifier")
    {
        errorMessage = "Expected table name after FROM";

        return false;
    }

    tableName = tokens[pos].value;

    pos++;

    // BUILD TABLE NODE
    QueryNode *tableNode = createNode("Table");

    tableNode->children.push_back(
        createNode("TableName", tableName));

    root->children.push_back(tableNode);

    if (!parseWhere())
    {
        return false;
    }

    // BUILD CONDITION NODE
    if (whereLeft != "")
    {
        QueryNode *conditionNode = createNode("Condition");

        QueryNode *leftNode = createNode("Left");

        leftNode->children.push_back(
            createNode("Identifier", whereLeft));

        QueryNode *operatorNode = createNode("Operator");

        operatorNode->children.push_back(
            createNode("Symbol", whereOperator));

        QueryNode *rightNode = createNode("Right");

        rightNode->children.push_back(
            createNode("Value", whereRight));

        conditionNode->children.push_back(leftNode);

        conditionNode->children.push_back(operatorNode);

        conditionNode->children.push_back(rightNode);

        root->children.push_back(conditionNode);
    }

    // optional semicolon
    if (pos < tokens.size() &&
        tokens[pos].value == ";")
    {
        pos++;
    }

    if (pos != tokens.size())
    {
        errorMessage = "Unexpected extra tokens";

        return false;
    }

    return true;
}

void tokenizer(string s)
{
    string updated = "";

    for (int i = 0; i < s.length(); i++)
    {
        // string literal
        if (s[i] == '\'')
        {
            string str = "'";

            i++;

            while (i < s.length() &&
                   s[i] != '\'')
            {
                str += s[i];

                i++;
            }

            str += "'";

            updated += " " + str + " ";

            continue;
        }

        // two character operators
        if (i + 1 < s.length())
        {
            string two = s.substr(i, 2);

            if (two == ">=" || two == "<=" ||
                two == "==" || two == "!=")
            {
                updated += " ";
                updated += two;
                updated += " ";

                i++;

                continue;
            }
        }

        // single character tokens
        if (s[i] == ',' || s[i] == ';' ||
            s[i] == '(' || s[i] == ')' ||
            s[i] == '+' || s[i] == '-' ||
            s[i] == '*' || s[i] == '/' ||
            s[i] == '=' || s[i] == '<' ||
            s[i] == '>')
        {
            updated += " ";
            updated += s[i];
            updated += " ";
        }

        else
        {
            updated += s[i];
        }
    }

    stringstream ss(updated);

    string word;

    while (ss >> word)
    {
        string temp = word;

        transform(temp.begin(), temp.end(),
                  temp.begin(), ::toupper);

        if (mysqlKeywords.count(temp))
        {
            tokens.push_back({temp, "keyword"});
        }

        else if (isIdentifier(word))
        {
            tokens.push_back({word, "identifier"});
        }

        else if (isNumber(word))
        {
            tokens.push_back({word, "digit"});
        }

        else if (word.front() == '\'' &&
                 word.back() == '\'')
        {
            tokens.push_back({word, "string"});
        }

        else if (word == ",")
        {
            tokens.push_back({word, "comma"});
        }

        else if (word == ";")
        {
            tokens.push_back({word, "semicolon"});
        }

        else if (word == "(")
        {
            tokens.push_back({word, "lparen"});
        }

        else if (word == ")")
        {
            tokens.push_back({word, "rparen"});
        }

        else if (ops.count(word))
        {
            tokens.push_back({word, "operator"});
        }

        else
        {
            tokens.push_back({word, "unknown"});
        }
    }
}

QueryNode *createNode(string type, string value)
{
    QueryNode *node = new QueryNode();

    node->type = type;
    node->value = value;

    return node;
}

void printTree(QueryNode *node, int depth)
{
    for (int i = 0; i < depth; i++)
    {
        cout << "  ";
    }

    cout << node->type;

    if (node->value != "")
    {
        cout << ": " << node->value;
    }

    cout << endl;

    for (QueryNode *child : node->children)
    {
        printTree(child, depth + 1);
    }
}

int main()
{
    string s;

    getline(cin, s);

    tokenizer(s);

    cout << "\nTOKENS\n\n";

    for (const auto &t : tokens)
    {
        cout << t.value
             << " -> "
             << t.type
             << endl;
    }

    cout << "\nPARSING RESULT\n\n";

    if (parseSelect())
    {
        cout << "Parsed Query\n";

        cout << "\nAST TREE\n\n";

        printTree(root);

        cout << "\nColumns:\n";

        if (selectAllColumns)
        {
            cout << "*\n";
        }

        else
        {
            for (int i = 0; i < selectedColumns.size(); i++)
            {
                cout << selectedColumns[i] << endl;
            }
        }

        cout << "\nTable:\n"
             << tableName << endl;

        cout << "\nCondition:\n";

        if (whereLeft != "")
        {
            cout << "left = "
                 << whereLeft << endl;

            cout << "operator = "
                 << whereOperator << endl;

            cout << "right = "
                 << whereRight << endl;
        }

        else
        {
            cout << "None" << endl;
        }
    }

    else
    {
        cout << "Syntax Error : "
             << errorMessage << endl;
    }

    return 0;
}