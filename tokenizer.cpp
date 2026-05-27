#include "tokenizer.h"
#include <algorithm>
#include <sstream>
#include <cctype>

vector<Token> tokens;

set<string> mysqlKeywords =
{
    "SELECT", "FROM", "WHERE",
    "INSERT", "UPDATE", "DELETE",
    "ORDER", "BY", "INTO",
    "VALUES", "SET"
};

set<string> ops =
{
    "+", "-", "*", "/",
    "=", "==", "!=", "<", ">",
    "<=", ">=", "%"
};

bool isNumber(string s)
{
    if(s.empty())
        return false;

    for(char ch : s)
    {
        if(!isdigit(ch))
            return false;
    }

    return true;
}

bool isIdentifier(string s)
{
    if(s.empty())
        return false;

    if(!(isalpha(s[0]) || s[0] == '_'))
        return false;

    for(int i = 1; i < s.length(); i++)
    {
        if(!(isalnum(s[i]) || s[i] == '_'))
            return false;
    }

    return true;
}

void tokenizer(string s)
{
    string updated = "";

    for(int i = 0; i < s.length(); i++)
    {
        // string literal
        if(s[i] == '\'')
        {
            string str = "'";

            i++;

            while(i < s.length() &&
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
        if(i + 1 < s.length())
        {
            string two = s.substr(i, 2);

            if(two == ">=" || two == "<=" ||
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
        if(s[i] == ',' || s[i] == ';' ||
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

    while(ss >> word)
    {
        string temp = word;

        transform(temp.begin(),
                  temp.end(),
                  temp.begin(),
                  ::toupper);

        if(mysqlKeywords.count(temp))
        {
            tokens.push_back(
                {temp, "keyword"});
        }

        else if(isIdentifier(word))
        {
            tokens.push_back(
                {word, "identifier"});
        }

        else if(isNumber(word))
        {
            tokens.push_back(
                {word, "digit"});
        }

        else if(word.front() == '\'' &&
                word.back() == '\'')
        {
            tokens.push_back(
                {word, "string"});
        }

        else if(word == ",")
        {
            tokens.push_back(
                {word, "comma"});
        }

        else if(word == ";")
        {
            tokens.push_back(
                {word, "semicolon"});
        }

        else if(word == "(")
        {
            tokens.push_back(
                {word, "lparen"});
        }

        else if(word == ")")
        {
            tokens.push_back(
                {word, "rparen"});
        }

        else if(ops.count(word))
        {
            tokens.push_back(
                {word, "operator"});
        }

        else
        {
            tokens.push_back(
                {word, "unknown"});
        }
    }
}