#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT_DIR="$ROOT_DIR/bin/renode"
OBJ_DIR="$OUT_DIR/obj"
LIB_DIR="$ROOT_DIR/lib/build/cortex-m3"
CXX="${CXX_RENODE:-arm-none-eabi-g++}"
CC="${CC_RENODE:-arm-none-eabi-gcc}"

if ! command -v "$CXX" >/dev/null || ! command -v "$CC" >/dev/null; then
    echo "arm-none-eabi-gcc and arm-none-eabi-g++ are required" >&2
    exit 1
fi

SYSROOT="$($CC -print-sysroot)"
if [[ ! -f "$SYSROOT/include/math.h" ]]; then
    echo "arm-none-eabi-newlib is required; install it with: sudo dnf install arm-none-eabi-newlib" >&2
    exit 1
fi

mkdir -p "$OBJ_DIR"
rm -f "$OBJ_DIR"/*.o "$OUT_DIR/htsp_renode.elf" "$OUT_DIR/htsp_renode.map"

"$ROOT_DIR/lib/build.sh" --arch=cortex-m3 --type=FIXED_POINT

C_COMMON_FLAGS=(
    -mcpu=cortex-m3
    -mthumb
    -mfloat-abi=soft
    -Os
    -Wall
    -Wextra
    -ffreestanding
    -ffunction-sections
    -fdata-sections
    -I"$ROOT_DIR/capi_v2"
    -I"$ROOT_DIR/src"
    -I"$ROOT_DIR/src/plugin"
    -I"$ROOT_DIR/src/pipeline"
    -I"$ROOT_DIR/src/module"
    -I"$ROOT_DIR/src/buffer"
    -I"$ROOT_DIR/src/interface"
    -I"$ROOT_DIR/utils"
)

CXX_FLAGS=(
    "${C_COMMON_FLAGS[@]}"
    -fno-exceptions
    -fno-rtti
    -fno-use-cxa-atexit
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -DFIXED_POINT
    -DRELEASE_BUILD
    -DRTAFE_BARE_METAL
)

"$CC" "${C_COMMON_FLAGS[@]}" -std=c99 -c "$ROOT_DIR/renode/startup.s" -o "$OBJ_DIR/startup.o"
"$CXX" "${CXX_FLAGS[@]}" -std=c++17 -c "$ROOT_DIR/renode/htsp_renode_main.cpp" -o "$OBJ_DIR/main.o"

"$CXX" \
    -mcpu=cortex-m3 -mthumb -mfloat-abi=soft \
    -nostartfiles -nostdlib \
    -T "$ROOT_DIR/renode/renode.ld" \
    -Wl,--gc-sections \
    -Wl,-Map="$OUT_DIR/htsp_renode.map" \
    "$OBJ_DIR"/*.o \
    "$LIB_DIR/librtafe.a" \
    -lgcc -lc -lnosys -lm \
    -o "$OUT_DIR/htsp_renode.elf"

arm-none-eabi-size "$OUT_DIR/htsp_renode.elf"
echo "Built $OUT_DIR/htsp_renode.elf"
