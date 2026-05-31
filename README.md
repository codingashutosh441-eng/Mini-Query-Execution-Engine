# Mini Query Execution Engine – Lightweight SQL Parser and Query Optimizer

Build a simplified SQL engine in C++ that accepts SQL queries, parses them, creates an execution plan, executes them on datasets, and visualizes the process through a React dashboard.

---

# Day 1

Created a tokenizer that takes SQL queries as input and converts them into tokens.

It also handles case-insensitive SQL keywords using strict parsing rules, meaning terms like `select`, `SELECT`, and `SelEct` are treated the same and never cause confusion in the system.

## Example

### Input
```sql
select * from abc where age >= 34;
```

### Output
```text
SELECT  -> keyword
*       -> operator
FROM    -> keyword
abc     -> identifier
WHERE   -> keyword
age     -> identifier
>=      -> operator
34      -> digit
;       -> semicolon
```

---

# Day 2

Added parsing support for a simple `SELECT` query grammar.

All generated tokens (stored as a `struct Token`) are inserted into a `vector`.  
Each token contains a key-value pair representing the token type and token value.

## Example Tokens

```text
SELECT  -> keyword
*       -> operator
FROM    -> keyword
abc     -> identifier
WHERE   -> keyword
age     -> identifier
>=      -> operator
34      -> digit
;       -> semicolon
```

## Parser Functions

### `parseSelect()`

A boolean function that checks whether the query starts with the `SELECT` keyword.

If valid, it moves the `pos` iterator forward by 1.

---

### `parseColumns()`

A boolean function that validates column names.

- If the token is `*`, it sets:

```cpp
selectAllColumns = true;
```

- If the token is an identifier:
  - Move the `pos` iterator forward.
  - Check for `,` between column names.
  - If a comma is missing where expected, return `false`.
  - Otherwise, continue parsing the next identifier.

---

### `parseFrom()`

A boolean function that checks whether the query contains the `FROM` keyword.

This helps identify the table name used in the query.

---

### `parseWhere()`

A boolean function that checks whether the query contains the `WHERE` keyword.

This helps validate conditions applied to table columns.

---


# Day 3

Implemented a basic SQL SELECT query parser with syntax validation and structured query extraction.

## Features Added

- Parsing SELECT queries
- Column list parsing
  - `SELECT *`
  - Multiple columns
- FROM clause parsing
- WHERE clause parsing
  - Supports:
    - `=`
    - `==`
    - `!=`
    - `<`
    - `>`
    - `<=`
    - `>=`
- Structured query output

## Example Supported Queries

```sql
SELECT * FROM students;

SELECT name, age FROM students;

SELECT name FROM students WHERE age >= 18;

Parsed Query

Columns:
name
age

Table:
students

Condition:
age >= 18

#Day 4
Implemented Abstract Syntax Tree (AST) generation for SQL queries.
Refactored parser into modular architecture with tokenizer, parser, and AST files.
Parser now directly builds tree-based query representations instead of storing flat parsed values.

SELECT name, age FROM students WHERE age >= 18;

TOKENS

SELECT -> keyword
name -> identifier
, -> comma
age -> identifier
FROM -> keyword
students -> identifier
WHERE -> keyword
age -> identifier
>= -> operator
18 -> digit
; -> semicolon

PARSING RESULT

Parsed Query

AST TREE

Query
  Columns
    Column: name
    Column: age
  Table: students
  Comparison
    Identifier: age
    Operator: >=
    Value: 18


# Day 4 - Database Layer & Query Execution

## Features Implemented

### In-Memory Database
Created:
- `database.h`
- `database.cpp`

Added:
- `Database`
- `Table`
- `Row`

Sample table:

| id | name  | age |
|----|-------|-----|
| 1  | Rahul | 20  |
| 2  | Amit  | 16  |
| 3  | Neha  | 22  |

---

### Executor Module
Created:
- `executor.h`
- `executor.cpp`

Implemented:
- SCAN
- FILTER
- PROJECT
- formatted result output

---

## Supported Queries

```sql
SELECT name FROM students WHERE age >= 18;
Supported operators:

>
<
>=
<=
==
!=
Example Output
RESULT

name
----------------
Rahul
Neha

## Features Added

- In-memory database layer
- Query executor module
- SCAN operation
- FILTER operation
- PROJECT operation
- Result table printing
- Multiple WHERE conditions
- AND / OR support
- String condition support

## Supported Queries

```sql
SELECT name FROM students WHERE age >= 18;

SELECT name FROM students
WHERE age >= 18 AND age < 25;

SELECT name FROM students
WHERE name == 'Rahul';