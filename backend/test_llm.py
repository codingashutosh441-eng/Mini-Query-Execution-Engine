from ai.schema import get_schema
from ai.prompt import build_prompt
from ai.llm import generate_sql

schema = get_schema()

prompt = build_prompt(
    "Show students older than 20",
    schema["text"]
)

sql = generate_sql(prompt)

print(sql)