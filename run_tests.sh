#!/bin/bash

echo "Compiling..."

g++ analyzer.cpp ast.cpp database.cpp executor.cpp main.cpp parser.cpp planner.cpp tokenizer.cpp \
    -std=c++17 -Wall -Wextra -g -o exec_test

if [ $? -ne 0 ]; then
    echo "Compilation failed"
    exit 1
fi

echo ""
echo "Running tests..."
echo ""

passed=0
failed=0

for sqlfile in $(find tests -name "*.sql" | sort)
do
    base="${sqlfile%.sql}"

    ./exec_test < "$sqlfile" > actual.txt

    if diff -q actual.txt "$base.expected" > /dev/null
    then
        echo "PASS $(basename "$base")"
        ((passed++))
    else
        echo "FAIL $(basename "$base")"
        echo ""

        echo "Differences:"
        diff "$base.expected" actual.txt

        echo ""
        ((failed++))
    fi
done

rm -f actual.txt

echo ""
echo "===================="
echo "Passed: $passed"
echo "Failed: $failed"
echo "===================="

if [ $failed -ne 0 ]; then
    exit 1
fi

exit 0