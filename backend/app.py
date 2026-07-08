from flask import Flask, request, jsonify
from flask_cors import CORS
import subprocess
from pathlib import Path
import os

PROJECT_ROOT = Path(__file__).resolve().parent.parent

app = Flask(__name__)
CORS(app)

exe_path = PROJECT_ROOT / "ash4.exe"

print("PROJECT =", PROJECT_ROOT)
print("EXE =", exe_path)
print("EXE EXISTS =", exe_path.exists())


@app.route("/query", methods=["POST"])
def query():

    sql = request.json["sql"]

    print("PROJECT =", PROJECT_ROOT)
    print(
        "DATABASE EXISTS =",
        os.path.exists(
            PROJECT_ROOT / "database"
        )
    )

    result = subprocess.run(
        [
            str(exe_path),
            sql
        ],
        capture_output=True,
        text=True,
        cwd=str(PROJECT_ROOT)
    )

    output = result.stdout

    # -------------------------
    # ERRORS
    # -------------------------

    if (
        "Semantic Error:" in output
        or "Syntax Error:" in output
        or "Duplicate value" in output
    ):

        error_message = output

        if "Duplicate value" in output:

            for line in output.splitlines():

                if (
                    "Duplicate value"
                    in line
                ):
                    error_message = (
                        line.strip()
                    )
                    break

        elif (
            "Semantic Error:"
            in output
        ):

            error_message = (
                output.split(
                    "Semantic Error:"
                )[1].strip()
            )

        elif (
            "Syntax Error:"
            in output
        ):

            error_message = (
                output.split(
                    "Syntax Error:"
                )[1].strip()
            )

        return jsonify({
            "success": False,
            "type": "error",
            "message": error_message
        })

    # -------------------------
    # EXPLAIN
    # -------------------------

    if (
        sql.strip()
        .upper()
        .startswith(
            "EXPLAIN"
        )
    ):

        lines = output.splitlines()

        plan_start = -1

        for i, line in enumerate(
            lines
        ):

            if (
                line.strip()
                == "EXECUTION PLAN"
            ):
                plan_start = i
                break

        if plan_start != -1:

            plan = []

            for line in lines[
                plan_start + 1:
            ]:

                line = line.strip()

                if not line:
                    continue

                if (
                    line
                    == "RESULT"
                ):
                    break

                plan.append(line)

            return jsonify({
                "success": True,
                "type": "plan",
                "plan": plan
            })

    # -------------------------
    # SELECT RESULT
    # -------------------------

    lines = output.splitlines()

    result_line = -1

    for i, line in enumerate(
        lines
    ):

        if (
            line.strip()
            == "RESULT"
        ):
            result_line = i
            break

    if result_line != -1:

        result_lines = []

        for line in lines[
            result_line + 1:
        ]:

            line = line.strip()

            if line:
                result_lines.append(
                    line
                )

        if (
            len(result_lines)
            >= 2
        ):

            columns = (
                result_lines[0]
                .split()
            )

            rows = []

            start_row = 1

            if (
                len(result_lines)
                > 1
                and "---"
                in result_lines[1]
            ):
                start_row = 2

            for line in result_lines[
                start_row:
            ]:

                rows.append(
                    line.split()
                )

            return jsonify({
                "success": True,
                "type": "result",
                "columns": columns,
                "rows": rows
            })

    # -------------------------
    # CREATE / INSERT /
    # UPDATE / DELETE /
    # CREATE INDEX etc.
    # -------------------------

    return jsonify({
        "success": (
            result.returncode == 0
        ),
        "type": "message",
        "message": output
    })


@app.route(
    "/tables",
    methods=["GET"]
)
def get_tables():

    database_path = (
        PROJECT_ROOT
        / "database"
    )

    tables = []

    for file in os.listdir(
        database_path
    ):

        if file.endswith(
            ".schema"
        ):

            tables.append(
                file.replace(
                    ".schema",
                    ""
                )
            )

    tables.sort()

    return jsonify(tables)


if __name__ == "__main__":
    app.run(
        port=5000,
        debug=True
    )