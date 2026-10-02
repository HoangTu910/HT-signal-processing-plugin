#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ARCH="host"
TYPE="FIXED_POINT"

for arg in "$@"; do
    case "$arg" in
        --arch=host|--arch=cortex-m3)
            ARCH="${arg#*=}"
            ;;
        --type=FIXED_POINT|--type=FLOATING_POINT)
            TYPE="${arg#*=}"
            ;;
        -h|--help)
            cat <<EOF
Usage: ./lib/build.sh [--arch=host|cortex-m3] [--type=FIXED_POINT|FLOATING_POINT]

Builds a static RTAFE library at lib/build/<arch>/librtafe.a.
EOF
            exit 0
            ;;
        *)
            echo "Unknown argument: $arg" >&2
            exit 1
            ;;
    esac
done

case "$ARCH" in
    host)
        CC="${CC:-gcc}"
        CXX="${CXX:-g++}"
        unset AS
        unset LD
        TARGET_FLAGS=(-O2 -B/usr/bin)
        ;;
    cortex-m3)
        CC="${CC_RENODE:-arm-none-eabi-gcc}"
        CXX="${CXX_RENODE:-arm-none-eabi-g++}"
        TARGET_FLAGS=(-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Os -ffreestanding)
        ;;
esac

if ! command -v "$CC" >/dev/null || ! command -v "$CXX" >/dev/null; then
    echo "Missing compiler for $ARCH: $CC / $CXX" >&2
    exit 1
fi

if [[ "$ARCH" == cortex-m3 ]]; then
    SYSROOT="$($CC -print-sysroot)"
    if [[ ! -f "$SYSROOT/include/math.h" ]]; then
        echo "arm-none-eabi-newlib is required; install it with: sudo dnf install arm-none-eabi-newlib" >&2
        exit 1
    fi
fi

OUT_DIR="$ROOT_DIR/lib/build/$ARCH"
OBJ_DIR="$OUT_DIR/obj"
mkdir -p "$OBJ_DIR"
rm -f "$OBJ_DIR"/*.o "$OUT_DIR/librtafe.a"

COMMON_FLAGS=(
    "${TARGET_FLAGS[@]}"
    -Wall
    -Wextra
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

if [[ "$TYPE" == FIXED_POINT ]]; then
    COMMON_FLAGS+=(-DFIXED_POINT)
fi

if [[ "$ARCH" == cortex-m3 ]]; then
    COMMON_FLAGS+=(
        -DRELEASE_BUILD
        -DRTAFE_BARE_METAL
    )
fi

CXX_FLAGS=(
    "${COMMON_FLAGS[@]}"
)

if [[ "$ARCH" == cortex-m3 ]]; then
    CXX_FLAGS+=(
        -fno-exceptions
        -fno-rtti
        -fno-use-cxa-atexit
        -fno-unwind-tables
        -fno-asynchronous-unwind-tables
    )
fi

C_SOURCES=(
    "$ROOT_DIR/src/module/fft.c"
)
CXX_SOURCES=(
    "$ROOT_DIR/src/plugin/htsp_plugin.cpp"
    "$ROOT_DIR/src/pipeline/dsp_pipeline.cpp"
    "$ROOT_DIR/src/module/dc_removal.cpp"
    "$ROOT_DIR/src/module/pre_emphasis.cpp"
    "$ROOT_DIR/src/module/noise_suppress.cpp"
    "$ROOT_DIR/src/interface/IRtafe_module.cpp"
    "$ROOT_DIR/src/buffer/buffer_manager.cpp"
    "$ROOT_DIR/src/buffer/dsp_block.cpp"
)

for source in "${C_SOURCES[@]}"; do
    object="$OBJ_DIR/$(basename "${source%.*}").o"
    "$CC" "${COMMON_FLAGS[@]}" -std=c99 -c "$source" -o "$object"
done

for source in "${CXX_SOURCES[@]}"; do
    object="$OBJ_DIR/$(basename "${source%.*}").o"
    "$CXX" "${CXX_FLAGS[@]}" -std=c++17 -c "$source" -o "$object"
done

ar rcs "$OUT_DIR/librtafe.a" "$OBJ_DIR"/*.o

echo "Built $OUT_DIR/librtafe.a (arch=$ARCH, type=$TYPE)"
