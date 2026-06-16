#include "tokenizer.h"
#include "parser.h"
#include "ast.h"
#include "planner.h"
#include "analyzer.h"
#include "database.h"
#include "executor.h"
#include "storage.h"
#include "command_handlers.h"
#include "config.h"

#include <iostream>
#include <string>

using namespace std;

int main()
{
    Database db;

    StorageManager::loadDatabase(
    db);

    // Read multiline query until ';'
    while (true)
    {
        string s;
        string line;

        cout << "\nminiSQL> ";

        while (getline(cin, line))
        {

            if (line == "EXIT" || line == "exit" || line == "quit" || line == "q")
            {
                return 0;
            }

            s += line;
            s += " ";

            if (line.find(';') != string::npos)
            {
                break;
            }
        }

        if (s.empty())
        {
            continue;
        }

        // TOKENIZATION
        tokenizer(s);

        if (DEBUG_MODE)
        {
            cout << "\nTOKENS\n\n";

            for (const auto &t : tokens)
            {
                cout << t.value
                     << " -> "
                     << t.type
                     << endl;
            }
        }
        if (tokens.empty())
        {
            continue;
        }

        // CREATE TABLE PATH
        Parser parser(tokens);
        SemanticAnalyzer analyzer(&db);

        string command = tokens[0].value;

        if (command == "CREATE")
        {
            handleCreate(
                parser,
                db);
            continue;
        }

        if (command == "INSERT")
        {

            handleInsert(
                parser,
                analyzer,
                db);

            continue;
        }

        if (command == "UPDATE")
        {
            handleUpdate(
                parser,
                analyzer,
                db);

            continue;
        }

        // SELECT
        if (command == "SELECT")
        {
            handleSelect(
                parser,
                analyzer,
                db);

            continue;
        }

        if (command == "DELETE")
        {
            handleDelete(
                parser,
                analyzer,
                db);

            continue;
        }
        cout << "Unknown command"
             << endl;
    }

    return 0;
}