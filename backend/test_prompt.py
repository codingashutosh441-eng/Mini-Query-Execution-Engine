from ai.schema import get_schema
from ai.prompt import build_prompt

schema = get_schema()

prompt = build_prompt(
    "Show students older than 20",
    schema["text"]
)

print(prompt)