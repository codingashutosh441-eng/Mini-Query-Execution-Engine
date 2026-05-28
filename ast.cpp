#include "ast.h"

void printTree(QueryNode* root)
{
    if(root == nullptr)
    {
        return;
    }

    cout << "Query" << endl;

    if(root->columns != nullptr)
    {
        cout << "  Columns" << endl;

        if(root->columns->selectAll)
        {
            cout << "    Column: *" << endl;
        }

        else
        {
            for(string column :
                root->columns->columns)
            {
                cout << "    Column: "
                     << column
                     << endl;
            }
        }
    }

    if(root->table != nullptr)
    {
        cout << "  Table: "
             << root->table->tableName
             << endl;
    }

    if(root->condition != nullptr)
    {
        cout << "  Condition" << endl;

        cout << "    Left: "
             << root->condition->left
             << endl;

        cout << "    Operator: "
             << root->condition->op
             << endl;

        cout << "    Right: "
             << root->condition->right
             << endl;
    }
}

void freeQuery(QueryNode* query)
{
    if(query == nullptr)
    {
        return;
    }

    delete query->columns;

    delete query->table;

    delete query->condition;

    delete query;
}