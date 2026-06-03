#include "tokenizer.h"
#include <algorithm>
#include <sstream>
#include <cctype>

vector<Token> tokens;

set<string> mysqlKeywords =
{
    "SELECT", "FROM", "WHERE",
    "INSERT", "UPDATE", "DELETE",
    "ORDER", "BY", "ASC", "DESC",
    "LIMIT",
    "INTO", "VALUES", "SET",
    "AND", "OR"
};

        
set<string> ops =
    {
        "+", "-", "*", "/",
        "=", "!=", "<", ">",
        "<=", ">=", "%"};

bool isNumber(string s)
{
    if (s.empty())
        return false;

    for (char ch : s)
    {
        if (!isdigit(ch))
            return false;
    }

    return true;
}

bool isIdentifier(string s)
{
    if (s.empty())
        return false;

    if (!(isalpha(s[0]) || s[0] == '_'))
        return false;

    for (size_t i = 1; i < s.length(); i++)
    {
        if (!(isalnum(s[i]) || s[i] == '_'))
            return false;
    }

    return true;
}

void tokenizer(string s)
{
    tokens.clear();

    string updated = "";

    for (size_t i = 0; i < s.length(); i++)
    {
        if (s[i] == '\'')
        {
            string str = "'";

            i++;

            while (i < s.length() &&
                   s[i] != '\'')
            {
                str += s[i];
                i++;
            }

            str += "'";

            updated += " " + str + " ";

            continue;
        }

        if (i + 1 < s.length())
        {
            string two = s.substr(i, 2);

            if (two == ">=" || two == "<=" ||
                two == "==" || two == "!=")
            {
                updated += " ";
                updated += two;
                updated += " ";

                i++;

                continue;
            }
        }

        if (s[i] == ',' || s[i] == ';' ||
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

    while (ss >> word)
    {
        string temp = word;

        transform(temp.begin(),
                  temp.end(),
                  temp.begin(),
                  ::toupper);

        // KEYWORDS

        if (mysqlKeywords.count(temp))
        {
            tokens.push_back(
                {temp, "keyword"});
        }

        // OPERATORS

        else if (ops.count(word))
        {
            tokens.push_back(
                {word, "operator"});
        }

        // NUMBERS

        else if (isNumber(word))
        {
            tokens.push_back(
                {word, "digit"});
        }

        // STRINGS

        else if (!word.empty() &&
                 word.front() == '\'' &&
                 word.back() == '\'')
        {
            tokens.push_back(
                {word, "string"});
        }

        // SYMBOLS

        else if (word == ",")
        {
            tokens.push_back(
                {word, "comma"});
        }

        else if (word == ";")
        {
            tokens.push_back(
                {word, "semicolon"});
        }

        else if (word == "(")
        {
            tokens.push_back(
                {word, "lparen"});
        }

        else if (word == ")")
        {
            tokens.push_back(
                {word, "rparen"});
        }

        // IDENTIFIERS

        else if (isIdentifier(word))
        {
            tokens.push_back(
                {word, "identifier"});
        }

        // UNKNOWN

        else
        {
            tokens.push_back(
                {word, "unknown"});
        }
    }
}