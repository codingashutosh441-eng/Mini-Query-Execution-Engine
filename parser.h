#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include "ast.h"

class Parser
{
private:
    const vector<Token> tokens;

    size_t pos = 0;

    string errorMessage;

    bool match(string expected);

    bool parseColumns(QueryNode *query);

    bool parseTable(QueryNode *query);

    bool parseWhere(QueryNode *query);
    bool parseOrderBy(QueryNode *query);
    bool parseLimit(QueryNode *query);
    ExpressionNode *parseConditionExpression();

    ExpressionNode *parsePrimary();

    ExpressionNode *parseAndExpression();

    ExpressionNode *parseExpression();

public:
    Parser(const vector<Token> tokenStream);

    QueryNode *parseSelect();

    string getError();
};

#endif