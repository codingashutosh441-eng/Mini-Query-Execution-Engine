#include <iostream>
#include <algorithm>
#include <sstream>
#include <set>
#include<regex>
using namespace std;

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

int main() {

    string s;

    getline(cin, s);

    stringstream ss(s);

    string word;

    regex identifier("^[a-zA-Z_][a-zA-Z0-9_]*$");
    regex number("^[0-9]+$");

    while(ss >> word) {

        string temp = word;

        transform(temp.begin(), temp.end(),
                  temp.begin(), ::toupper);

        if(mysqlKeywords.count(temp)) {
            cout << word << " -> Keyword" << endl;
        }

        else if(regex_match(word, identifier)) {
            cout << word << " -> Identifier" << endl;
        }

        else if(regex_match(word, number)) {
            cout << word << " -> Number" << endl;
        }

        else if(word == ",") {
            cout << word << " -> Comma" << endl;
        }

        else if(word == ";") {
            cout << word << " -> Semicolon" << endl;
        }

        else {
            cout << word << " -> Unknown" << endl;
        }
    }

    return 0;
}