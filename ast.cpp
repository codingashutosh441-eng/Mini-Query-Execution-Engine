#include "ast.h"

QueryNode* createNode(
    string type,
    string value)
{
    QueryNode* node =
        new QueryNode();

    node->type = type;
    node->value = value;

    return node;
}

void printTree(
    QueryNode* node,
    int depth)
{
    for(int i = 0; i < depth; i++)
    {
        cout << "  ";
    }

    cout << node->type;

    if(node->value != "")
    {
        cout << ": "
             << node->value;
    }

    cout << endl;

    for(QueryNode* child :
        node->children)
    {
        printTree(
            child,
            depth + 1);
    }
}