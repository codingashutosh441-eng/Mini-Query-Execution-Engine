from ollama import chat
from ai.config import MODEL_NAME


def generate_sql(prompt: str) -> str:
    """
    Sends the prompt to Gemma and returns
    the raw response.
    """

    response = chat(
        model=MODEL_NAME,
        messages=[
            {
                "role": "user",
                "content": prompt
            }
        ]
    )

    return response.message.content.strip()