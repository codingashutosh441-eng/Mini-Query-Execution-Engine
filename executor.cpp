#include "executor.h"
#include <iostream>

using namespace std;

Executor::Executor(Database* database)
{
    db = database;
}



bool Executor::evaluateCondition(Row row,
                                 ConditionNode* condition)
{
    if(condition == nullptr)
    {
        return true;
    }

    string left = condition->left;
    string op = condition->op;
    string right = condition->right;

    // AGE
    if(left == "age")
    {
        int value = stoi(right);

        if(op == ">")
        {
            return row.age > value;
        }

        else if(op == "<")
        {
            return row.age < value;
        }

        else if(op == ">=")
        {
            return row.age >= value;
        }

        else if(op == "<=")
        {
            return row.age <= value;
        }

        else if(op == "==" || op == "=")
        {
            return row.age == value;
        }

        else if(op == "!=")
        {
            return row.age != value;
        }
    }

    // ID
    else if(left == "id")
    {
        int value = stoi(right);

        if(op == ">")
        {
            return row.id > value;
        }

        else if(op == "<")
        {
            return row.id < value;
        }

        else if(op == ">=")
        {
            return row.id >= value;
        }

        else if(op == "<=")
        {
            return row.id <= value;
        }

        else if(op == "==" || op == "=")
        {
            return row.id == value;
        }

        else if(op == "!=")
        {
            return row.id != value;
        }
    }

    // NAME
    else if(left == "name")
    {
        if(op == "==" || op == "=")
        {
            return row.name == right;
        }

        else if(op == "!=")
        {
            return row.name != right;
        }
    }

    return false;
}


void Executor::execute(QueryNode* query)
{
    Table* table =
        db->getTable(query->table->tableName);

    if(table == nullptr)
    {
        cout << "Table not found" << endl;

        return;
    }

    cout << "\nRESULT\n\n";

    // PRINT HEADERS

    if(query->columns->selectAll)
    {
        cout << "id\tname\tage" << endl;
        cout << "------------------------" << endl;
    }

    else
    {
        for(const auto& column :
            query->columns->columns)
        {
            cout << column << "\t";
        }

        cout << endl;
        cout << "------------------------" << endl;
    }

    // PRINT ROWS

    for(const auto& row : table->rows)
    {
        // APPLY FILTER

        if(!evaluateCondition(row,
                              query->condition))
        {
            continue;
        }

        // SELECT *

        if(query->columns->selectAll)
        {
            cout << row.id << "\t"
                 << row.name << "\t"
                 << row.age
                 << endl;
        }

        // PROJECT COLUMNS

        else
        {
            for(const auto& column :
                query->columns->columns)
            {
                if(column == "id")
                {
                    cout << row.id << "\t";
                }

                else if(column == "name")
                {
                    cout << row.name << "\t";
                }

                else if(column == "age")
                {
                    cout << row.age << "\t";
                }
            }

            cout << endl;
        }
    }
}