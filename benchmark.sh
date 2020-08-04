#!/usr/bin/env bash

which hyperfine >/dev/null || { echo "hyperfine must be installed!" && exti 1; }
which jql >/dev/null || { echo "jql must be installed!" && exti 1; }

echo "Make sure you compiled ./jsonquery with RELEASE=1 to create comparable results!"

RESULT_FILE=benchmarks/README.md

mkdir -p benchmarks
echo > "$RESULT_FILE"

append_result() {
    echo "# $1" >> "$RESULT_FILE"
    echo >> "$RESULT_FILE"
    echo "<details>" >> "$RESULT_FILE"
    echo "<summary>$1.json</summary>" >> "$RESULT_FILE"
    echo >> "$RESULT_FILE"
    echo '```json' >> "$RESULT_FILE"
    cat "test/$1.json" >> "$RESULT_FILE"
    echo '```' >> "$RESULT_FILE"
    echo >> "$RESULT_FILE"
    echo "</details>" >> "$RESULT_FILE"
    echo >> "$RESULT_FILE"
    cat "benchmarks/$1.md" >> "$RESULT_FILE"
    echo >> "$RESULT_FILE"
    echo >> "$RESULT_FILE"
}

# test/laptops.json

hyperfine \
    --warmup 5 \
    --export-markdown benchmarks/simple.md \
    "./jsonquery '\"name\"' test/simple.json" \
    "jql '\"name\"' test/simple.json" \
    "jq '.name' test/simple.json"

append_result "simple"

hyperfine \
    --warmup 5 \
    --export-markdown benchmarks/arrays.md \
    "./jsonquery '\"dna\"[1:3]' test/arrays.json" \
    "jql '\"dna\"[1:3]' test/arrays.json" \
    "jq '.dna[1:3]' test/arrays.json"

append_result "arrays"

# TODO modify this to flatten array
hyperfine \
    --warmup 5 \
    --export-markdown benchmarks/generated.md \
    "./jsonquery '[:].\"friends\"' test/generated.json" \
    "jql '[:]|\"friends\"' test/generated.json" \
    "jq '[.[] | .friends]' test/generated.json"

append_result "generated"

hyperfine \
    --warmup 5 \
    --export-markdown benchmarks/generated.md \
    "./jsonquery '..\"tags\"' test/generated.json" \
    "jql '..[:]|\"tags\"' test/generated.json" \
    "jq '[.[] | .tags[]]' test/generated.json"

append_result "generated"

echo "> Results are in $RESULT_FILE"
