#include "tokenizer.h"
#include "parser.h"
#include "ast.h"
#include "planner.h"
#include "analyzer.h"
#include "database.h"
#include "executor.h"

int main()
{
    string s;

    getline(cin, s);

    if (s.empty())
    {
        cout << "Empty query" << endl;

        return 0;
    }
    Database db;
    db.seedStudents();

    tokenizer(s);

    cout << "\nTOKENS\n\n";

    for (const auto &t : tokens)
    {
        cout << t.value
             << " -> "
             << t.type
             << endl;
    }

    Parser parser(tokens);

    QueryNode *query =
        parser.parseSelect();

    cout << "\nPARSING RESULT\n\n";

    if (query != nullptr)
    {
        cout << "Parsed Query\n";

        cout << "\nAST TREE\n\n";

        printTree(query);

        SemanticAnalyzer analyzer;
        if (!analyzer.validate(query))
        {
            cout << "\nSemantic Error : "
                 << analyzer.errorMessage
                 << endl;

            freeQuery(query);

            return 0;
        }

        Planner planner;
        planner.createPlan(query);
        planner.printPlan();
        Executor executor(&db);
        executor.execute(query);

        freeQuery(query);

    }

    else
    {
        cout << "Syntax Error : "
             << parser.getError()
             << endl;
    }

    return 0;
}