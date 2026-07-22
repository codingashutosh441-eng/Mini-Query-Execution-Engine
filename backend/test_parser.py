from ai.parser import clean_sql

examples = [

"""```sql
SELECT * FROM students;
```""",

"""Here is the SQL query:

SELECT * FROM students;
""",

"""SQL:

SELECT * FROM students;
""",

"""SELECT * FROM students;"""
]

for example in examples:

    print("------------------")
    print(clean_sql(example))