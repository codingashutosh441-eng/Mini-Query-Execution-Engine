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
