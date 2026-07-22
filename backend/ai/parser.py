import re


def clean_sql(response: str) -> str:
    """
    Cleans the LLM response and extracts SQL.
    """

    # Remove markdown fences
    response = response.replace("```sql", "")
    response = response.replace("```", "")

    # Remove common prefixes
    prefixes = [
        "Here is the SQL query:",
        "SQL:",
        "Query:"
    ]

    for prefix in prefixes:
        if response.startswith(prefix):
            response = response[len(prefix):]

    response = response.strip()

    # Keep only the first SQL statement
    match = re.search(r".*?;", response, re.DOTALL)

    if match:
        return match.group(0).strip()

    return response