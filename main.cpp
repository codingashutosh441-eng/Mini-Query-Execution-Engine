#include "tokenizer.h"
#include "parser.h"
#include "ast.h"
#include "planner.h"
#include "analyzer.h"
#include "database.h"
#include "executor.h"

#include <iostream>
#include <string>

using namespace std;

const bool DEBUG_MODE = false;

int main()
{
    string s;
    string line;

    // Read multiline query until ';'
    while (getline(cin, line))
    {
        if (line.empty())
        {
            break;
        }
        s += line;
        s += " ";

        if (line.find(';') != string::npos)
        {
            break;
        }
    }

    if (s.empty())
    {
        cout << "Empty query" << endl;
        return 0;
    }

    // DATABASE SETUP
    Database db;
    db.seedStudents();

    // TOKENIZATION
    tokenizer(s);

    if (DEBUG_MODE)
    {
        cout << "\nTOKENS\n\n";

        for (const auto &t : tokens)
        {
            cout << t.value
                 << " -> "
                 << t.type
                 << endl;
        }
    }

    // CREATE TABLE PATH

    if (!tokens.empty() &&
        tokens[0].value == "CREATE")
    {
        Parser parser(tokens);

        CreateTableNode *createNode =
            parser.parseCreateTable();

        if (createNode == nullptr)
        {
            cout << "Syntax Error: "
                 << parser.getError()
                 << endl;

            return 0;
        }

        vector<ColumnInfo> columns;

        for (const auto &col : createNode->columns)
        {
            columns.push_back(
                {col.name,
                 col.type});
        }

        bool success =
            db.createSchema(
                createNode->tableName,
                columns);

        if (!success)
        {
            cout << "ERROR: Table '"
                 << createNode->tableName
                 << "' already exists"
                 << endl;

            delete createNode;

            return 0;
        }

        cout << "Table created successfully"
             << endl
             << endl;

        cout << createNode->tableName
             << endl
             << endl;

        for (const auto &col : createNode->columns)
        {
            cout << col.name << "\t";

            if (col.type == DataType::INT)
            {
                cout << "INT";
            }
            else
            {
                cout << "STRING";
            }

            cout << endl;
        }

        delete createNode;

        return 0;
    }

    // SELECT PATH (existing code)

    // PARSING
    Parser parser(tokens);

    // CREATE TABLE
    if (!tokens.empty() &&
        tokens[0].value == "CREATE")
    {
        CreateTableNode *createNode =
            parser.parseCreateTable();

        if (createNode == nullptr)
        {
            cout << "Syntax Error: "
                 << parser.getError()
                 << endl;

            return 0;
        }

        vector<ColumnInfo> columns;

        for (const auto &col :
             createNode->columns)
        {
            ColumnInfo info;

            info.name = col.name;
            info.type = col.type;

            columns.push_back(info);
        }

        bool created =
            db.createSchema(
                createNode->tableName,
                columns);

        if (!created)
        {
            cout << "Table already exists: "
                 << createNode->tableName
                 << endl;

            delete createNode;

            return 0;
        }

        cout << "Table created successfully"
             << endl
             << endl;

        cout << createNode->tableName
             << endl
             << endl;

        for (const auto &col :
             createNode->columns)
        {
            cout << col.name << "\t";

            if (col.type == DataType::INT)
            {
                cout << "INT";
            }
            else
            {
                cout << "STRING";
            }

            cout << endl;
        }

        delete createNode;

        return 0;
    }

    // SELECT
    QueryNode *query =
        parser.parseSelect();

    if (query == nullptr)
    {
        cout << "Syntax Error: "
             << parser.getError()
             << endl;

        return 0;
    }

    if (DEBUG_MODE)
    {
        cout << "\nPARSING RESULT\n\n";

        cout << "Parsed Query\n";

        cout << "\nAST TREE\n\n";

        printTree(query);
    }

    // SEMANTIC ANALYSIS
    SemanticAnalyzer analyzer(&db);

    if (!analyzer.validate(query))
    {
        cout << "Semantic Error:\n"
             << analyzer.errorMessage
             << endl;

        freeQuery(query);

        return 0;
    }

    // QUERY PLANNER
    Planner planner;

    planner.createPlan(query);

    if (DEBUG_MODE)
    {
        cout << "\nSemantic Analysis Passed\n";

        cout << "\nQUERY PLAN\n\n";

        planner.printPlan();
    }

    // EXECUTION
    Executor executor(&db);

    executor.execute(query);

    // CLEANUP
    freeQuery(query);

    return 0;
}