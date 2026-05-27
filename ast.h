#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct QueryNode
{
    string type;
    string value;

    vector<QueryNode*> children;
};

// AST functions
QueryNode* createNode(
    string type,
    string value = "");

void printTree(
    QueryNode* node,
    int depth = 0);

#endif