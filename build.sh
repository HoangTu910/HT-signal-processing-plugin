#!/usr/bin/env bash

set -euo pipefail

ARCH="x86"
TYPE="FLOATING_POINT"

for arg in "$@"; do
    case "$arg" in
        --arch=*)
            ARCH="${arg#*=}"
            ;;
        --type=*)
            TYPE="${arg#*=}"
            ;;
        -h|--help)
            cat <<EOF
Usage: ./build.sh [--arch=x86|arm] [--type=FIXED_POINT|FLOATING_POINT]

Builds the WAV processing plugin binary from src/test_plugin_main.cpp.

Examples:
  ./build.sh
  ./build.sh --arch=x86 --type=FLOATING_POINT
  ./build.sh --arch=arm --type=FIXED_POINT
EOF
            exit 0
            ;;
        *)
            echo "Unknown argument: $arg"
            exit 1
            ;;
    esac
done

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_DIR="$ROOT_DIR/src"
WAV_DIR="$SRC_DIR/wav"
PLUGIN_DIR="$SRC_DIR/plugin"
MODULE_DIR="$SRC_DIR/module"
BUFFER_DIR="$SRC_DIR/buffer"
INTERFACE_DIR="$SRC_DIR/interface"
UTILS_DIR="$ROOT_DIR/utils"
BUILD_DIR="$ROOT_DIR/bin"
OBJ_DIR="$BUILD_DIR/obj"

mkdir -p "$BUILD_DIR" "$OBJ_DIR"

CC="${CC:-gcc}"
CXX="${CXX:-g++}"

case "$ARCH" in
    x86)
        OUT="$BUILD_DIR/test_plugin_main"
        # Prefer the native assembler when PATH contains a cross-toolchain.
        unset AS
        unset LD
        TARGET_FLAGS="-B/usr/bin"
        ;;
    arm)
        OUT="$BUILD_DIR/test_plugin_main_arm"
        CC="${CC:-aarch64-linux-gnu-gcc}"
        CXX="${CXX:-aarch64-linux-gnu-g++}"
        unset AS
        TARGET_FLAGS="-march=armv8-a+simd -ffast-math -DARM_TARGET"
        ;;
    *)
        echo "Unsupported arch: $ARCH"
        exit 1
        ;;
esac

case "$TYPE" in
    FIXED_POINT)
        TYPE_FLAGS="-DFIXED_POINT"
        echo "WARNING: Fixed-point build is experimental and may produce unstable output."
        ;;
    FLOATING_POINT)
        TYPE_FLAGS=""
        ;;
    *)
        echo "Unsupported type: $TYPE"
        exit 1
        ;;
esac

COMMON_FLAGS=(
    -O2
    -Wall
    -Wextra
    -I"$ROOT_DIR"
    -I"$SRC_DIR"
    -I"$WAV_DIR"
    -I"$PLUGIN_DIR"
    -I"$MODULE_DIR"
    -I"$BUFFER_DIR"
    -I"$INTERFACE_DIR"
    -I"$UTILS_DIR"
)

if [ -n "$TYPE_FLAGS" ]; then
    COMMON_FLAGS+=("$TYPE_FLAGS")
fi

if [ -n "$TARGET_FLAGS" ]; then
    COMMON_FLAGS+=("$TARGET_FLAGS")
fi

C_SRCS=(
    "$ROOT_DIR/utils/fft.c"
)

CXX_SRCS=(
    "$SRC_DIR/test_plugin_main.cpp"
    "$PLUGIN_DIR/rtafe_plugin.cpp"
    "$MODULE_DIR/dc_removal.cpp"
    "$MODULE_DIR/pre_emphasis.cpp"
    "$MODULE_DIR/noise_suppress.cpp"
    "$INTERFACE_DIR/IRtafe_module.cpp"
    "$BUFFER_DIR/buffer_manager.cpp"
    "$BUFFER_DIR/dsp_block.cpp"
    "$WAV_DIR/wav_file_mgr.cpp"
)

rm -f "$OUT"
rm -f "$OBJ_DIR"/*.o

echo "Building $OUT (arch=$ARCH, type=$TYPE)"

for src in "${C_SRCS[@]}"; do
    obj="$OBJ_DIR/$(basename "${src%.*}").o"
    "$CC" -std=c99 "${COMMON_FLAGS[@]}" -c "$src" -o "$obj"
done

for src in "${CXX_SRCS[@]}"; do
    obj="$OBJ_DIR/$(basename "${src%.*}").o"
    "$CXX" -std=c++17 "${COMMON_FLAGS[@]}" -c "$src" -o "$obj"
done

"$CXX" "${TARGET_FLAGS}" "$OBJ_DIR"/*.o -o "$OUT" -lm -lstdc++

echo "Done: $OUT"