# MiniSQL – Custom SQL Query Engine

MiniSQL is a lightweight SQL query execution engine built from scratch using **C++**, with a web-based frontend for executing and viewing SQL queries.

The project implements the core stages of SQL query processing, including **lexical analysis, parsing, semantic analysis, query planning, query execution, and CSV-based storage**.

The goal of the project is to understand how a database engine processes a SQL query internally rather than relying on an existing database system.

---

## Features

MiniSQL currently supports the following SQL operations and query-processing capabilities:

* `CREATE TABLE`
* `INSERT`
* `SELECT`
* `UPDATE`
* `DELETE`
* `WHERE`
* `GROUP BY`
* `LIMIT`
* Aggregate functions
* Sorting using `ORDER BY`
* Filtering
* CSV-based persistent storage

The query engine is implemented through the following processing components:

* Lexer / Tokenizer
* Parser
* Abstract Syntax Tree (AST)
* Semantic Analyzer
* Query Planner
* Query Executor
* Storage Manager

A web-based frontend is also provided for interacting with the SQL engine.

---

## Architecture

```text
                         ┌──────────────────┐
                         │     Frontend     │
                         │   Web Interface  │
                         └────────┬─────────┘
                                  │
                                  │ SQL Query
                                  ▼
                         ┌──────────────────┐
                         │     Backend      │
                         └────────┬─────────┘
                                  │
                                  ▼
                         ┌──────────────────┐
                         │      Lexer       │
                         │    Tokenizer     │
                         └────────┬─────────┘
                                  │
                                Tokens
                                  │
                                  ▼
                         ┌──────────────────┐
                         │      Parser      │
                         └────────┬─────────┘
                                  │
                                  ▼
                         ┌──────────────────┐
                         │       AST        │
                         └────────┬─────────┘
                                  │
                                  ▼
                     ┌────────────────────────┐
                     │   Semantic Analyzer    │
                     └───────────┬────────────┘
                                 │
                                 ▼
                     ┌────────────────────────┐
                     │     Query Planner      │
                     └───────────┬────────────┘
                                 │
                                 ▼
                     ┌────────────────────────┐
                     │     Query Executor     │
                     └───────────┬────────────┘
                                 │
                                 ▼
                     ┌────────────────────────┐
                     │    Storage Manager     │
                     │     CSV Storage        │
                     └───────────┬────────────┘
                                 │
                                 ▼
                            Query Result
```

---

## Query Execution Pipeline

A SQL query submitted through the frontend passes through multiple stages before the result is returned.

```text
SQL Query
    │
    ▼
Lexer / Tokenizer
    │
    ▼
Parser
    │
    ▼
AST
    │
    ▼
Semantic Analyzer
    │
    ▼
Query Planner
    │
    ▼
Query Executor
    │
    ▼
Storage Manager
    │
    ▼
Query Result
```

### 1. Lexer

The lexer converts the SQL query into a sequence of tokens.

For example:

```sql
SELECT name FROM students WHERE age > 18;
```

is broken into tokens representing keywords, identifiers, operators, literals, and other SQL elements.

```text
SELECT
IDENTIFIER(name)
FROM
IDENTIFIER(students)
WHERE
IDENTIFIER(age)
GREATER_THAN
NUMBER(18)
```

---

### 2. Parser

The parser consumes the tokens produced by the lexer and checks them against the supported SQL grammar.

It builds an **Abstract Syntax Tree (AST)** representing the structure of the SQL query.

Conceptually:

```text
SELECT
├── Columns
│   └── name
├── Table
│   └── students
└── WHERE
    └── age > 18
```

---

### 3. Abstract Syntax Tree

The AST provides a structured representation of the SQL query.

Instead of executing the raw SQL string directly, later stages operate on the parsed query representation.

This separates SQL syntax handling from semantic validation and execution.

---

### 4. Semantic Analysis

The semantic analyzer validates the parsed query before execution.

It checks the query against the available database schema and ensures that the requested operations are valid.

Examples include:

* Checking whether referenced tables exist.
* Checking whether referenced columns exist.
* Validating query structure.
* Validating operations against the available schema.

---

### 5. Query Planning

The query planner converts the validated query into an execution plan.

For example:

```sql
SELECT name
FROM students
WHERE age > 18
LIMIT 5;
```

can conceptually be represented as:

```text
Scan Table
    │
    ▼
Filter age > 18
    │
    ▼
Project name
    │
    ▼
Limit 5
```

The query executor then processes these operations.

---

### 6. Query Execution

The query executor executes the generated query plan.

Depending on the query, execution can involve:

* Reading records.
* Filtering records.
* Sorting records.
* Grouping records.
* Performing aggregation.
* Limiting results.
* Inserting records.
* Updating records.
* Deleting records.

---

### 7. Storage Manager

MiniSQL uses **CSV files as the persistent storage layer**.

The Storage Manager handles reading and writing table data so that records can persist between executions.

The query executor communicates with the storage layer when it needs to retrieve or modify table data.

---

# Supported SQL

## CREATE TABLE

Creates a table with a specified schema.

```sql
CREATE TABLE students (
    id INT,
    name VARCHAR,
    age INT
);
```

---

## INSERT

Adds records to a table.

```sql
INSERT INTO students VALUES (1, 'Alice', 20);

INSERT INTO students VALUES (2, 'Bob', 22);

INSERT INTO students VALUES (3, 'Charlie', 19);
```

---

## SELECT

Retrieves records from a table.

```sql
SELECT *
FROM students;
```

Specific columns can also be selected:

```sql
SELECT name, age
FROM students;
```

---

## WHERE

Filters records based on a condition.

```sql
SELECT name, age
FROM students
WHERE age >= 20;
```

Filtering can be used with other query operations such as sorting and limiting.

---

## GROUP BY

Groups records based on one or more columns.

For example:

```sql
SELECT age, COUNT(*)
FROM students
GROUP BY age;
```

`GROUP BY` can be combined with aggregate functions to perform calculations on groups of records.

---

## Aggregation

MiniSQL supports aggregate operations for processing multiple records.

Supported aggregate functions include:

```text
COUNT
SUM
AVG
MIN
MAX
```

Example:

```sql
SELECT COUNT(*)
FROM students;
```

Another example using grouping:

```sql
SELECT age, COUNT(*)
FROM students
GROUP BY age;
```

---

## Sorting

Query results can be sorted using `ORDER BY`.

```sql
SELECT name, age
FROM students
ORDER BY age;
```

---

## LIMIT

`LIMIT` restricts the number of records returned by a query.

```sql
SELECT *
FROM students
LIMIT 2;
```

---

## UPDATE

Updates existing records.

```sql
UPDATE students
SET age = 21
WHERE id = 1;
```

---

## DELETE

Deletes records from a table.

```sql
DELETE FROM students
WHERE id = 3;
```

---

# Frontend

MiniSQL includes a web-based frontend that provides an interface for interacting with the SQL engine.

The frontend allows users to:

* Write SQL queries.
* Execute queries.
* Send queries to the backend.
* View query results.
* View execution errors.

The frontend communicates with the backend through the API layer.

```text
User
 │
 ▼
SQL Editor
 │
 ▼
Backend API
 │
 ▼
MiniSQL Engine
 │
 ▼
Query Result
 │
 ▼
Frontend
```

---

# Project Structure

```text
Mini-Query-Execution-Engine/
│
├── backend/
│   ├── ...
│   └── app.py
│
├── frontend/
│   ├── src/
│   ├── public/
│   ├── package.json
│   └── ...
│
├── data/
│   └── ...
│
└── README.md
```

The backend contains the SQL engine and API, while the frontend provides the user interface for executing queries.

---

# Installation

## Clone the Repository

```bash
git clone https://github.com/codingashutosh441-eng/Mini-Query-Execution-Engine.git

cd Mini-Query-Execution-Engine
```

## Start Backend

Open a terminal and run:

```bash
cd backend

python app.py
```

The backend API will start and handle SQL requests from the frontend.

## Start Frontend

Open another terminal:

```bash
cd frontend

npm install

npm run dev
```

Open the local URL provided by the frontend development server in your browser.

---

# Example Workflow

A typical MiniSQL workflow looks like this:

### 1. Create a table

```sql
CREATE TABLE students (
    id INT,
    name VARCHAR,
    age INT
);
```

### 2. Insert records

```sql
INSERT INTO students VALUES (1, 'Alice', 20);
INSERT INTO students VALUES (2, 'Bob', 22);
INSERT INTO students VALUES (3, 'Charlie', 19);
```

### 3. Query the data

```sql
SELECT name, age
FROM students
WHERE age >= 20
ORDER BY age
LIMIT 2;
```

The query is processed as:

```text
SQL Query
    ↓
Lexer
    ↓
Parser
    ↓
AST
    ↓
Semantic Analysis
    ↓
Query Planning
    ↓
Query Execution
    ↓
CSV Storage
    ↓
Result
```

---

# Technology Stack

### Database Engine

* C++
* C++ STL
* CSV-based storage

### Backend

* Python
* Flask

### Frontend

* React
* JavaScript
* HTML
* CSS

---

# Project Goals

The main goal of MiniSQL is to understand database internals by implementing a simplified SQL engine from scratch.

The project focuses on understanding how a database system:

1. Receives a SQL query.
2. Tokenizes the query.
3. Parses the SQL grammar.
4. Builds an AST.
5. Validates the query semantically.
6. Creates an execution plan.
7. Executes the planned operations.
8. Reads and modifies persistent data.
9. Returns the final result.

---

# Current Limitations

MiniSQL is an educational SQL engine and is not intended to be a production database system.

The current implementation focuses on the SQL operations and query-processing capabilities listed in this README.

Advanced database features such as transactions, concurrency control, crash recovery, distributed execution, and advanced query optimization are outside the current scope.

---

# Future Improvements

Potential improvements include:

* More SQL operations and expressions.
* Improved query optimization.
* Indexing.
* Transaction support.
* Concurrency control.
* More advanced storage structures.
* Query performance benchmarking.
* Expanded automated testing.

---

# Status

**Active Development**

MiniSQL currently provides a functional SQL query engine with a multi-stage query-processing pipeline, CSV-based persistent storage, backend API, and web-based frontend.
