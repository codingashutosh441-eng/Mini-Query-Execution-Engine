#include "ast.h"

void printTree(QueryNode *root)
{
    if (root == nullptr)
    {
        return;
    }

    cout << "Query" << endl;

    if (root->columns != nullptr)
    {
        cout << "  Columns" << endl;

        if (root->columns->selectAll)
        {
            cout << "    Column: *" << endl;
        }

        else
        {
            for (string column :
                 root->columns->columns)
            {
                cout << "    Column: "
                     << column
                     << endl;
            }
        }
    }

    if (root->table != nullptr)
    {
        cout << "  Table: "
             << root->table->tableName
             << endl;
    }

    if (root->whereExpression != nullptr)
    {
        cout << "Expression Tree" << endl;

        printExpressionTree(
            root->whereExpression,
            1);
    }

    if (root->orderBy != nullptr)
    {
        cout << "  OrderBy" << endl;

        cout << "    Column: "
             << root->orderBy->column
             << endl;

        cout << "    Direction: "
             << root->orderBy->direction
             << endl;
    }

    if (root->limit != nullptr)
    {
        cout << "  Limit" << endl;

        cout << "    Count: "
             << root->limit->count
             << endl;
    }
}

void printExpressionTree(ExpressionNode *node, int level)
{
    if (node == nullptr)
    {
        return;
    }

    for (int i = 0; i < level; i++)
    {
        cout << "  ";
    }

    if (node->isLogical)
    {
        cout << node->logicalOp << endl;
    }
    else
    {
        cout
            << node->column
            << " "
            << node->op
            << " "
            << node->value
            << endl;
    }

    printExpressionTree(node->left, level + 1);
    printExpressionTree(node->right, level + 1);
}

void freeExpressionTree(ExpressionNode *node)
{
    if (node == nullptr)
    {
        return;
    }

    freeExpressionTree(node->left);

    freeExpressionTree(node->right);

    delete node;
}

void freeQuery(QueryNode *query)
{
    if (query == nullptr)
    {
        return;
    }

    delete query->columns;

    delete query->table;

    delete query->orderBy;

    delete query->limit;

    freeExpressionTree(
        query->whereExpression);

    delete query;
}