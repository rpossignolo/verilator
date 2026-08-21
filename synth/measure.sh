#!/usr/bin/env bash
# Verilate a synthetic with a given verilator, then COLD-compile the trace TUs
# (ccache bypassed) and report trace object bytes + compile wall.
set -euo pipefail
VLT="${1:?verilator bin}"; SRC="${2:?src.sv}"; TOP="${3:?top}"; TAG="${4:?tag}"
OBJ="obj_${TAG}"
DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$DIR"
rm -rf "${OBJ:?}"
"$VLT" --cc --trace-fst -O3 --trace-max-width 512 -Mdir "$OBJ" "$SRC" &> "vlt_${TAG}.log"
# Real g++, not the ccache shim on PATH.
GXX=/usr/bin/g++
INC="$(dirname "$($VLT --getenv VERILATOR_ROOT 2>/dev/null || echo /usr)")"
VROOT="$($VLT --getenv VERILATOR_ROOT)"
CXXFLAGS="-I$OBJ -I$VROOT/include -I$VROOT/include/vltstd -O2 -std=gnu++20 -c"
tbytes=0; t0=$(date +%s%N)
for f in "$OBJ"/*Trace*.cpp; do
  o="${f%.cpp}.o"
  $GXX $CXXFLAGS "$f" -o "$o"
  sz=$(stat -c%s "$o"); tbytes=$((tbytes+sz))
done
t1=$(date +%s%N)
echo "TAG=$TAG trace_obj_bytes=$tbytes cold_compile_ms=$(( (t1-t0)/1000000 ))"
