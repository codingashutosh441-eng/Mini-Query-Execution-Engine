#include <iostream>
#include <algorithm>
#include <sstream>
#include <set>
#include <vector>
#include <cctype>

using namespace std;

struct Token {
    string value;
    string type;
};

vector<Token> tokens;
int pos = 0;
bool selectAllColumns = false;

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

    "SELECT", "FROM", "WHERE",
    "INSERT", "UPDATE", "DELETE",
    "ORDER", "BY", "INTO",
    "VALUES", "SET"
};

set<string> ops = {

    "+", "-", "*", "/",
    "=", "==", "!=", "<", ">",
    "<=", ">=", "%"
};

bool expect(string val) {

    if(pos < tokens.size() &&
       tokens[pos].value == val) {

        pos++;
        return true;
    }

    return false;
}

bool parseColumns() {

    if(pos >= tokens.size())
        return false;

    if(tokens[pos].value == "*"){
        selectAllColumns = true;
        pos++;
        return true;
    }

    if(tokens[pos].type != "identifier")
        return false;

    pos++;

    while(pos < tokens.size() &&
          tokens[pos].value == ",") {

        pos++;

        if(pos >= tokens.size())
            return false;

        if(tokens[pos].type != "identifier")
            return false;

        pos++;
    }

    return true;
}

bool parseWhere() {

    if(pos < tokens.size() &&
       tokens[pos].value == "WHERE") {

        pos++;

        if(pos >= tokens.size() ||
           tokens[pos].type != "identifier")
            return false;

        pos++;

        if(pos >= tokens.size() ||
           tokens[pos].type != "operator")
            return false;

        pos++;

        if(pos >= tokens.size())
            return false;

        if(tokens[pos].type != "digit" &&
           tokens[pos].type != "string" &&
           tokens[pos].type != "identifier")
            return false;

        pos++;
    }

    return true;
}

bool parseSelect() {

    if(!expect("SELECT")) {
        cout << "Syntax Error : SELECT expected\n";
        return false;
    }

    if(!parseColumns()) {
        cout << "Syntax Error : Invalid column list\n";
        return false;
    }

    if(!expect("FROM")) {
        cout << "Syntax Error : FROM expected\n";
        return false;
    }

    if(pos >= tokens.size() ||
       tokens[pos].type != "identifier") {

        cout << "Syntax Error : Table name expected\n";
        return false;
    }

    pos++;

    if(!parseWhere()) {
        cout << "Syntax Error : Invalid WHERE clause\n";
        return false;
    }

    if(pos < tokens.size() &&
       tokens[pos].value == ";") {

        pos++;
    }

    if(pos != tokens.size()) {
        cout << "Syntax Error : Extra tokens found\n";
        return false;
    }

    return true;
}

int main() {

    string s;

    getline(cin, s);
    string updated = "";

    for(int i = 0; i < s.length(); i++) {

        if(s[i] == '\'') {

            string str = "'";
            i++;

            while(i < s.length() &&
                  s[i] != '\'') {

                str += s[i];
                i++;
            }

            str += "'";
            updated += " " + str + " ";
            continue;
        }

        if(i + 1 < s.length()) {

            string two = s.substr(i, 2);

            if(two == ">=" || two == "<=" ||
               two == "==" || two == "!=") {

                updated += " ";
                updated += two;
                updated += " ";

                i++;
                continue;
            }
        }

        if(s[i] == ',' || s[i] == ';' ||
           s[i] == '(' || s[i] == ')' ||
           s[i] == '+' || s[i] == '-' ||
           s[i] == '*' || s[i] == '/' ||
           s[i] == '=' || s[i] == '<' ||
           s[i] == '>') {

            updated += " ";
            updated += s[i];
            updated += " ";
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

            tokens.push_back({temp, "keyword"});
        }

        else if(isIdentifier(word)) {

            tokens.push_back({word, "identifier"});
        }

        else if(isNumber(word)) {

            tokens.push_back({word, "digit"});
        }

        else if(word.front() == '\'' &&
                word.back() == '\'') {

            tokens.push_back({word, "string"});
        }

        else if(word == ",") {

            tokens.push_back({word, "comma"});
        }

        else if(word == ";") {

            tokens.push_back({word, "semicolon"});
        }

        else if(word == "(") {

            tokens.push_back({word, "lparen"});
        }

        else if(word == ")") {

            tokens.push_back({word, "rparen"});
        }

        else if(ops.count(word)) {

            tokens.push_back({word, "operator"});
        }

        else {

            tokens.push_back({word, "unknown"});
        }
    }

    cout << "\nTOKENS\n\n";

    for(const auto& t : tokens) {

        cout << t.value
             << " -> "
             << t.type
             << endl;
    }

    cout << "\nPARSING RESULT\n\n";

    if(parseSelect()) {

        cout << "Valid SELECT Query\n";
    }

    else {

        cout << "Invalid Query\n";
    }

    return 0;
}