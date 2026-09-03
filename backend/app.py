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


# ============================================================
# RUN MINI SQL
# ============================================================

def execute_sql(sql):

    result = subprocess.run(
        [
            str(exe_path),
            sql
        ],
        capture_output=True,
        text=True,
        cwd=str(PROJECT_ROOT)
    )

    return result


# ============================================================
# QUERY ENDPOINT
# ============================================================

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

    result = execute_sql(sql)

    output = result.stdout

    # --------------------------------------------------------
    # ERRORS
    # --------------------------------------------------------

    if (
        "Semantic Error:" in output
        or "Syntax Error:" in output
        or "Duplicate value" in output
    ):

        error_message = output

        if "Duplicate value" in output:

            for line in output.splitlines():

                if "Duplicate value" in line:
                    error_message = line.strip()
                    break

        elif "Semantic Error:" in output:

            error_message = (
                output.split(
                    "Semantic Error:"
                )[1].strip()
            )

        elif "Syntax Error:" in output:

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

    # --------------------------------------------------------
    # EXPLAIN
    # --------------------------------------------------------

    if (
        sql.strip()
        .upper()
        .startswith("EXPLAIN")
    ):

        lines = output.splitlines()

        plan_start = -1

        for i, line in enumerate(lines):

            if line.strip() == "EXECUTION PLAN":
                plan_start = i
                break

        if plan_start != -1:

            plan = []

            for line in lines[plan_start + 1:]:

                line = line.strip()

                if not line:
                    continue

                if line == "RESULT":
                    break

                plan.append(line)

            return jsonify({
                "success": True,
                "type": "plan",
                "plan": plan
            })

    # --------------------------------------------------------
    # SELECT RESULT
    # --------------------------------------------------------

    lines = output.splitlines()

    result_line = -1

    for i, line in enumerate(lines):

        if line.strip() == "RESULT":
            result_line = i
            break

    if result_line != -1:

        result_lines = []

        for line in lines[result_line + 1:]:

            line = line.strip()

            if line:
                result_lines.append(line)

        if len(result_lines) >= 2:

            columns = result_lines[0].split()

            rows = []

            start_row = 1

            if (
                len(result_lines) > 1
                and "---" in result_lines[1]
            ):
                start_row = 2

            for line in result_lines[start_row:]:

                rows.append(
                    line.split()
                )

            return jsonify({
                "success": True,
                "type": "result",
                "columns": columns,
                "rows": rows
            })

    # --------------------------------------------------------
    # CREATE / INSERT / UPDATE / DELETE
    # --------------------------------------------------------

    return jsonify({
        "success": (
            result.returncode == 0
        ),
        "type": "message",
        "message": output
    })


# ============================================================
# TEST PLAN
# ============================================================

@app.route("/test-plan", methods=["POST"])
def test_plan():

    TEST_TABLE = "minisql_test"

    tests = []

    # --------------------------------------------------------
    # Helper function
    # --------------------------------------------------------

    def run_test(
        name,
        sql,
        expected_type,
        expected_text=None
    ):

        result = execute_sql(sql)

        output = result.stdout.strip()

        # Check for MiniSQL errors

        is_error = (
            "Semantic Error:" in output
            or "Syntax Error:" in output
            or "Duplicate value" in output
            or "Unknown command" in output
        )

        passed = False

        # --------------------------------------------
        # Expected successful query
        # --------------------------------------------

        if expected_type == "success":

            passed = (
                result.returncode == 0
                and not is_error
            )

        # --------------------------------------------
        # Expected error
        # --------------------------------------------

        elif expected_type == "error":

            passed = is_error

        # --------------------------------------------
        # Expected text in output
        # --------------------------------------------

        elif expected_type == "contains":

            passed = (
                not is_error
                and expected_text.lower()
                in output.lower()
            )

        # --------------------------------------------
        # Expected empty result
        # --------------------------------------------

        elif expected_type == "empty":

            passed = (
                not is_error
                and "RESULT" in output
            )

            # If RESULT exists, check whether
            # there are actual data rows.

            if passed:

                lines = output.splitlines()

                result_index = -1

                for i, line in enumerate(lines):

                    if line.strip() == "RESULT":
                        result_index = i
                        break

                if result_index != -1:

                    result_lines = [
                        line.strip()
                        for line in lines[
                            result_index + 1:
                        ]
                        if line.strip()
                    ]

                    # Header + separator means
                    # there are no actual rows.

                    if len(result_lines) >= 2:

                        if "---" in result_lines[1]:

                            data_rows = result_lines[2:]

                            passed = (
                                len(data_rows) == 0
                            )

                        else:
                            passed = False

        tests.append({
            "name": name,
            "sql": sql,
            "passed": passed,
            "expected": (
                expected_text
                if expected_text
                else expected_type
            ),
            "actual": output
        })

    # ========================================================
    # 1. CREATE
    # ========================================================

    run_test(
        "CREATE",
        (
            f"CREATE TABLE {TEST_TABLE} "
            "(id INT PRIMARY KEY, name string);"
        ),
        "success"
    )

    # ========================================================
    # 2. INSERT
    # ========================================================

    run_test(
        "INSERT",
        (
            f"INSERT INTO {TEST_TABLE} "
            "VALUES (1, 'Alice');"
        ),
        "success"
    )

    # ========================================================
    # 3. SELECT
    # ========================================================

    run_test(
        "SELECT",
        f"SELECT * FROM {TEST_TABLE};",
        "contains",
        "Alice"
    )

    # ========================================================
    # 4. UPDATE
    # ========================================================

    run_test(
        "UPDATE",
        (
            f"UPDATE {TEST_TABLE} "
            "SET name = 'Bob' WHERE id = 1;"
        ),
        "success"
    )

    # ========================================================
    # 5. SELECT AFTER UPDATE
    # ========================================================

    run_test(
        "SELECT AFTER UPDATE",
        f"SELECT * FROM {TEST_TABLE};",
        "contains",
        "Bob"
    )

    # ========================================================
    # 6. DELETE
    # ========================================================

    run_test(
        "DELETE",
        (
            f"DELETE FROM {TEST_TABLE} "
            "WHERE id = 1;"
        ),
        "success"
    )

    # ========================================================
    # 7. EMPTY TABLE
    # ========================================================

    run_test(
        "EMPTY TABLE",
        f"SELECT * FROM {TEST_TABLE};",
        "empty"
    )

    # ========================================================
    # 8. INVALID SQL
    # ========================================================

    run_test(
        "INVALID SQL",
        f"SELEC * FROM {TEST_TABLE};",
        "error"
    )

    # ========================================================
    # 9. NON-EXISTING TABLE
    # ========================================================

    run_test(
        "NON-EXISTING TABLE",
        "SELECT * FROM table_that_does_not_exist;",
        "error"
    )

    # ========================================================
    # SUMMARY
    # ========================================================

    passed_count = sum(
        1
        for test in tests
        if test["passed"]
    )

    failed_count = (
        len(tests) - passed_count
    )

    return jsonify({
        "success": failed_count == 0,
        "total": len(tests),
        "passed": passed_count,
        "failed": failed_count,
        "tests": tests
    })


# ============================================================
# GET DATABASE TABLES
# ============================================================

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

    if not database_path.exists():
        return jsonify(tables)

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


# ============================================================
# START FLASK
# ============================================================

if __name__ == "__main__":

    app.run(
        port=5000,
        debug=True
    )