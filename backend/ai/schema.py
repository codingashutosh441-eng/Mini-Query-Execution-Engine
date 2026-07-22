from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent.parent
DATABASE_PATH = PROJECT_ROOT / "database"

def get_schema(): 
    """
    Reads all .schema files from the database directory.

    Returns:
        {
            "text": Formatted schema string for the LLM,
            "tables": Structured schema metadata
        }
    """
    schema_lines = []
    tables = {}

    for schema_file in sorted(DATABASE_PATH.glob("*.schema")):

        table_name = schema_file.stem
        tables[table_name] = []

        schema_lines.append(f"TABLE {table_name}")

        with open(schema_file, "r") as f:

            for line in f:

                line = line.strip()

                if not line:
                    continue

                column, datatype, constraint = line.split(",")

                tables[table_name].append({
                    "column": column,
                    "type": datatype,
                    "constraint": constraint
                })

                schema_lines.append(
                    f"    {column} {datatype}"
                )

        schema_lines.append("")

    return {
        "text": "\n".join(schema_lines),
        "tables": tables
    }
