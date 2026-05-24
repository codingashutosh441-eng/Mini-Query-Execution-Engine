#include <iostream>
#include <algorithm>
#include <sstream>
#include <set>
#include<cctype>
using namespace std;


bool isNumber(string s) {

    if(s.empty())
        return false;

    for(char ch : s) {

        if(!isdigit(ch))
            return false;
    }

    return true;
}
bool isIdentifier(string s) {

    if(s.empty())
        return false;

    if(!(isalpha(s[0]) || s[0] == '_'))
        return false;

    for(int i = 1; i < s.length(); i++) {

        if(!(isalnum(s[i]) || s[i] == '_'))
            return false;
    }

    return true;
}

set<string> mysqlKeywords = {

    "ADD", "ALL", "ALTER", "ANALYZE", "AND",
    "AS", "ASC", "AUTO_INCREMENT",
    "BETWEEN", "BY",
    "CASE", "CHECK", "COLUMN", "CONSTRAINT",
    "CREATE", "CROSS",
    "DATABASE", "DEFAULT", "DELETE", "DESC",
    "DISTINCT", "DROP",
    "ELSE", "END", "EXISTS",
    "FOREIGN", "FROM", "FULL",
    "GROUP",
    "HAVING",
    "IN", "INDEX", "INNER", "INSERT", "INTO",
    "IS",
    "JOIN",
    "KEY",
    "LEFT", "LIKE", "LIMIT",
    "NOT", "NULL",
    "ON", "OR", "ORDER", "OUTER",
    "PRIMARY", "PROCEDURE",
    "REFERENCES", "RIGHT", "ROLLBACK",
    "SELECT", "SET",
    "TABLE", "THEN", "TO", "TRANSACTION",
    "TRUNCATE",
    "UNION", "UNIQUE", "UPDATE", "USE",
    "VALUES", "VIEW",
    "WHEN", "WHERE"

};
set<string> ops = {
        "+", "-", "*", "/",
        "=", "==", "!=", "<", ">",
        "<=", ">=", "&&", "||", "%"
};
int main() {

    string s;

    getline(cin, s);
    

    string updated = "";

    for(int i = 0; i < s.length(); i++) {

       if(i + 1 < s.length()) {

            string two = s.substr(i, 2);

            if(two == ">=" || two == "<=" ||
            two == "==" || two == "!=" ||
            two == "<>") {

            updated += ' ';
            updated += two;
            updated += ' ';

            i++; // skip next character
            continue;
           }
        }

        if(s[i] == ',' || s[i] == ';' ||
           s[i] == '(' || s[i] == ')' ||
           s[i] == '+' || s[i] == '-' ||
           s[i] == '*' || s[i] == '/' ||
           s[i] == '=' || s[i] == '<' ||
           s[i] == '>') {

           updated += ' ';
           updated += s[i];
           updated += ' ';
        }
        else {
           updated += s[i];
        }
    }

    stringstream ss(updated);

    string word;
    while(ss >> word) {

        string temp = word;

        transform(temp.begin(), temp.end(),
                  temp.begin(), ::toupper);

        if(mysqlKeywords.count(temp)) {
            cout << word << " -> Keyword" << endl;
        }

        else if(isIdentifier(word)) {
            cout << word << " -> Identifier" << endl;
        }

        else if(isNumber(word)){
             cout << word << " -> Digit" << endl;
        }

        else if(word == ",") {
            cout << word << " -> Comma" << endl;
        }

        else if(word == ";") {
            cout << word << " -> Semicolon" << endl;
        }

        else if(ops.count(word)){
            cout << word << " -> operator" << endl;
        }
        else{
            cout << word << " -> Unknown" << endl;
        }
    }

    return 0;
}