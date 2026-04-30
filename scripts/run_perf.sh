#!/bin/bash
set -e

BINARY="./build/order_book"
OUTPUT="perf_report"

echo "=== Building in Release mode ==="
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-g" > /dev/null
cmake --build build > /dev/null

echo "=== Running perf stat ==="
perf stat -e task-clock,page-faults,context-switches,cpu-migrations \
    $BINARY 2>&1 | tee ${OUTPUT}_stat.txt

echo "=== Running perf record ==="
perf record -g -o perf.data $BINARY

echo "=== Generating perf report ==="
perf report --stdio -i perf.data > ${OUTPUT}_hotspots.txt 2>&1

echo ""
echo "Results written to:"
echo "  ${OUTPUT}_stat.txt     (cycle/cache counts)"
echo "  ${OUTPUT}_hotspots.txt (hotspot functions)"