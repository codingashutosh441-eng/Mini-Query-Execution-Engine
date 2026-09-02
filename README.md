# MiniSQL – Custom SQL Query Engine

MiniSQL is a lightweight SQL query execution engine built from scratch using **C++**, with a **Python backend** and **React frontend**.

It implements the complete query-processing flow from **lexical analysis to query execution**, using CSV files for persistent storage.

---

## Features

* `CREATE TABLE`
* `INSERT`
* `SELECT`
* `UPDATE`
* `DELETE`
* `WHERE`
* `GROUP BY`
* `LIMIT`
* Aggregate functions: `COUNT`, `SUM`, `AVG`, `MIN`, `MAX`
* Sorting using `ORDER BY`
* CSV-based storage
* Lexer / Tokenizer
* Parser and AST
* Semantic Analysis
* Query Planning
* Query Execution
* Web-based SQL interface

---

## Architecture

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
CSV Storage
    │
    ▼
Query Result
```

The **React frontend** sends SQL queries to the **Python backend**, which passes them to the MiniSQL engine.

---

## Supported SQL

### Create Table

```sql
CREATE TABLE students (
    id INT,
    name VARCHAR,
    age INT
);
```

### Insert

```sql
INSERT INTO students VALUES (1, 'Alice', 20);
```

### Select with Filtering

```sql
SELECT name, age
FROM students
WHERE age >= 20;
```

### Grouping and Aggregation

```sql
SELECT age, COUNT(*)
FROM students
GROUP BY age;
```

### Sorting and Limit

```sql
SELECT name, age
FROM students
ORDER BY age
LIMIT 5;
```

### Update

```sql
UPDATE students
SET age = 21
WHERE id = 1;
```

### Delete

```sql
DELETE FROM students
WHERE id = 1;
```

---

## Project Structure

```text
Mini-Query-Execution-Engine/
│
├── tokenizer.cpp / .h
├── parser.cpp / .h
├── ast.cpp / .h
├── analyzer.cpp / .h
├── planner.cpp / .h
├── executor.cpp / .h
├── storage.cpp / .h
├── database.cpp / .h
├── command_handlers.cpp / .h
├── main.cpp
│
├── database/
├── backend/
├── frontend/
├── run_tests.sh
└── README.md
```

---

## Installation

### Clone

```bash
git clone https://github.com/codingashutosh441-eng/Mini-Query-Execution-Engine.git
cd Mini-Query-Execution-Engine
```

### Start Backend

```bash
cd backend
python app.py
```

### Start Frontend

Open another terminal:

```bash
cd frontend
npm install
npm run dev
```

Open the local URL provided by the frontend development server.

---

## Tech Stack

* **C++** — SQL engine
* **Python / Flask** — Backend API
* **React / JavaScript** — Frontend
* **CSV** — Persistent storage

---

## Project Goal

The goal of MiniSQL is to understand how a relational database processes SQL queries internally by implementing the core components from scratch:

**Lexer → Parser → AST → Semantic Analysis → Query Planning → Query Execution → Storage**

---

## Status

**Active Development**

MiniSQL currently supports the SQL operations and query-processing features listed above.
