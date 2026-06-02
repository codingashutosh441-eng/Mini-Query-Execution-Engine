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

    // PARSING
    Parser parser(tokens);

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