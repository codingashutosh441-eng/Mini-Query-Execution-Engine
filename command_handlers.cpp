#include "command_handlers.h"
#include "storage.h"
#include "config.h"
#include "planner.h"
#include "executor.h"
#include "ast.h"

#include <iostream>

using namespace std;

void handleCreate(
    Parser &parser,
    Database &db)
{
    CreateTableNode *createNode =
        parser.parseCreateTable();

    if (createNode == nullptr)
    {
        cout << "Syntax Error: "
             << parser.getError()
             << endl;

        return;
    }

    vector<ColumnInfo> columns;

    for (const auto &col : createNode->columns)
    {
        ColumnInfo info;

        info.name = col.name;
        info.type = col.type;
        info.primaryKey = col.isPrimaryKey;
        info.unique = col.isUnique;
        cout
            << info.name
            << " PK=" << info.primaryKey
            << " UNIQUE=" << info.unique
            << endl;

        columns.push_back(info);
    }

    bool success =
        db.createSchema(
            createNode->tableName,
            columns);

    if (!success)
    {
        cout << "ERROR: Table '"
             << createNode->tableName
             << "' already exists"
             << endl;

        delete createNode;

        return;
    }
    for (const auto &col :
         columns)
    {
        if (col.primaryKey ||
            col.unique)
        {
            db.createIndex(
                createNode->tableName,
                col.name);

            StorageManager::saveIndex(
                createNode->tableName,
                col.name);
        }
    }

    const TableSchema *schema =
        db.getSchema(
            createNode->tableName);

    StorageManager::createTableFiles(
        *schema);

    cout << "Table created successfully"
         << endl
         << endl;

    cout << createNode->tableName
         << endl
         << endl;

    for (const auto &col : createNode->columns)
    {
        cout << col.name << "\t";

        if (col.type == DataType::INT)
        {
            cout << "INT";
        }
        else
        {
            cout << "STRING";
        }

        cout << endl;
    }

    delete createNode;
}

void handleInsert(
    Parser &parser,
    SemanticAnalyzer &analyzer,
    Database &db)
{
    InsertNode *insertNode =
        parser.parseInsert();

    if (insertNode == nullptr)
    {
        cout << "Syntax Error: "
             << parser.getError()
             << endl;

        return;
    }

    if (!analyzer.validateInsert(insertNode))
    {
        cout << "Semantic Error:\n"
             << analyzer.errorMessage
             << endl;

        delete insertNode;

        return;
    }

    Executor executor(&db);

    executor.executeInsert(insertNode);

    delete insertNode;
}

void handleUpdate(
    Parser &parser,
    SemanticAnalyzer &analyzer,
    Database &db)
{
    UpdateNode *updateNode =
        parser.parseUpdate();

    if (!updateNode)
    {
        cout << "Syntax Error: "
             << parser.getError()
             << endl;

        return;
    }

    if (!analyzer.validateUpdate(updateNode))
    {
        cout << "Semantic Error:\n"
             << analyzer.errorMessage
             << endl;

        delete updateNode;
        return;
    }

    Executor executor(&db);

    executor.executeUpdate(
        updateNode);

    delete updateNode;
}

void handleSelect(
    Parser &parser,
    SemanticAnalyzer &analyzer,
    Database &db)
{
    QueryNode *query =
        parser.parseSelect();

    if (query == nullptr)
    {
        cout << "Syntax Error: "
             << parser.getError()
             << endl;

        return;
    }

    if (DEBUG_MODE)
    {
        cout << "\nPARSING RESULT\n\n";

        cout << "Parsed Query\n";

        cout << "\nAST TREE\n\n";

        printTree(query);
    }

    if (!analyzer.validate(query))
    {
        cout << "Semantic Error:\n"
             << analyzer.errorMessage
             << endl;

        freeQuery(query);

        return;
    }

    // QUERY PLANNER
    Planner planner(&db);

    planner.createPlan(query);
    if (query->explainMode)
    {
        planner.printPlan();

        freeQuery(query);

        return;
    }

    if (DEBUG_MODE)
    {
        cout << "\nSemantic Analysis Passed\n";

        cout << "\nQUERY PLAN\n\n";

        planner.printPlan();
    }

    // EXECUTION
    Executor executor(&db);

    executor.execute(query);

    // CLEANUP
    freeQuery(query);
}

void handleDelete(
    Parser &parser,
    SemanticAnalyzer &analyzer,
    Database &db)
{
    DeleteNode *deleteNode =
        parser.parseDelete();

    if (!deleteNode)
    {
        cout << "Syntax Error: "
             << parser.getError()
             << endl;

        return;
    }

    if (!analyzer.validateDelete(
            deleteNode))
    {
        cout << "Semantic Error:\n"
             << analyzer.errorMessage
             << endl;

        freeDelete(deleteNode);
        return;
    }

    Executor executor(&db);

    executor.executeDelete(
        deleteNode);

    freeDelete(deleteNode);
}

void handleCreateIndex(
    Parser &parser,
    SemanticAnalyzer &analyzer,
    Database &db)
{
    CreateIndexNode *node =
        parser.parseCreateIndex();

    if (!node)
    {
        cout << "Syntax Error: "
             << parser.getError()
             << endl;

        return;
    }

    if (!analyzer.validateCreateIndex(node))
    {
        cout << "Semantic Error:\n"
             << analyzer.errorMessage
             << endl;

        delete node;
        return;
    }

    bool success =
        db.createIndex(
            node->tableName,
            node->columnName);

    if (success)
    {
        StorageManager::saveIndex(
            node->tableName,
            node->columnName);
        cout
            << "Index created successfully"
            << endl;
    }
    else
    {
        cout
            << "Index already exists"
            << endl;
    }

    delete node;
}