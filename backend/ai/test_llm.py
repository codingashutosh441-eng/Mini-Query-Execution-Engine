from ollama import chat
from config import MODEL_NAME
from backend.ai.prompt import build_prompt


def generate_sql(question: str):
    prompt = build_prompt(question)

    response = chat(
        model=MODEL_NAME,
        messages=[
            {
                "role": "user",
                "content": prompt
            }
        ]
    )

    return response["message"]["content"]


def main():
    print("=" * 60)
    print("MiniSQL AI - Text to SQL")
    print("=" * 60)

    while True:
        question = input("\nAsk a question (type 'exit' to quit): ")

        if question.lower() == "exit":
            break

        try:
            sql = generate_sql(question)

            print("\nGenerated SQL:\n")
            print(sql)

        except Exception as e:
            print("\nError:")
            print(e)


if __name__ == "__main__":
    main()