def build_prompt(question: str, schema: str) -> str:
    """
    Builds a prompt for the LLM using the
    user's question and the database schema.
    """

    return f"""
You are an expert SQL generator.

Your task is to convert a user's natural language request into SQL.

Rules:
1. Return ONLY SQL.
2. Do NOT explain your answer.
3. Do NOT use Markdown.
4. Do NOT wrap the SQL inside ```sql.
5. Use ONLY the tables and columns provided.
6. Never invent tables.
7. Never invent columns.
8. End every query with a semicolon.
9. If the request cannot be answered using the schema, return exactly:
   INVALID_QUERY

Database Schema:

{schema}

User Question:
{question}

SQL:
"""