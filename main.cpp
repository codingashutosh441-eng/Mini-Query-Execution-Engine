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

void executeQuery(
    const string &s,
    Database &db)
{
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
        return;
    }

    Parser parser(tokens);
    SemanticAnalyzer analyzer(&db);

    string command = tokens[0].value;

    if (command == "CREATE")
    {
        if (tokens.size() > 1 &&
            tokens[1].value == "TABLE")
        {
            handleCreate(parser, db);
        }
        else if (tokens.size() > 1 &&
                 tokens[1].value == "INDEX")
        {
            handleCreateIndex(
                parser,
                analyzer,
                db);
        }

        return;
    }

    if (command == "INSERT")
    {
        handleInsert(
            parser,
            analyzer,
            db);

        return;
    }

    if (command == "UPDATE")
    {
        handleUpdate(
            parser,
            analyzer,
            db);

        return;
    }

    if (command == "SELECT" ||
        command == "EXPLAIN")
    {
        handleSelect(
            parser,
            analyzer,
            db);

        return;
    }

    if (command == "DELETE")
    {
        handleDelete(
            parser,
            analyzer,
            db);

        return;
    }

    cout << "Unknown command"
         << endl;
}

int main(int argc, char *argv[])
{
    Database db;

    db.createIndex(
        "students",
        {"age"});

    StorageManager::loadDatabase(
        db);

    if (DEBUG_MODE)
{
    cout << "Loaded tables:" << endl;

    for (const auto &pair :
         db.getSchemas())
    {
        cout << pair.first << endl;
    }
}

    StorageManager::loadIndexes(
        db);

    if (argc > 1)
    {
        if (DEBUG_MODE)
{
    cout << "ARGUMENT RECEIVED: "
         << argv[1]
         << endl;
}
        string query = argv[1];

        executeQuery(
            query,
            db);

        return 0;
    }

    // Read multiline query until ';'
    while (true)
    {
        string s;
        string line;

        cout << "\nminiSQL> ";

        while (getline(cin, line))
        {
            if (line == "EXIT" ||
                line == "exit" ||
                line == "quit" ||
                line == "q")
            {
                return 0;
            }

            s += line;
            s += " ";

            if (line.find(';') !=
                string::npos)
            {
                break;
            }
        }

        if (s.empty())
        {
            continue;
        }

        executeQuery(
            s,
            db);
    }

    return 0;
}