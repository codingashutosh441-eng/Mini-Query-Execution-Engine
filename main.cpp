#include "tokenizer.h"
#include "parser.h"
#include "ast.h"

int main()
{
    string s;

    getline(cin, s);

    tokenizer(s);

    cout << "\nTOKENS\n\n";

    for(const auto &t : tokens)
    {
        cout << t.value
             << " -> "
             << t.type
             << endl;
    }

    cout << "\nPARSING RESULT\n\n";

    if(parseSelect())
    {
        cout << "Parsed Query\n";

        cout << "\nAST TREE\n\n";

        printTree(root);
    }

    else
    {
        cout << "Syntax Error : "
             << errorMessage
             << endl;
    }

    return 0;
}