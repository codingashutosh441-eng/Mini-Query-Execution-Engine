#ifndef COMMAND_HANDLERS_H
#define COMMAND_HANDLERS_H

#include "parser.h"
#include "analyzer.h"
#include "database.h"

void handleCreate(
    Parser& parser,
    Database& db);

void handleInsert(
    Parser& parser,
    SemanticAnalyzer& analyzer,
    Database& db);

void handleUpdate(
    Parser& parser,
    SemanticAnalyzer& analyzer,
    Database& db);

void handleSelect(
    Parser& parser,
    SemanticAnalyzer& analyzer,
    Database& db);

#endif