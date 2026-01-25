#!/usr/bin/env bash
set -e

MCU="$1"
if [ -z "$MCU" ]; then
  echo "ERROR: MCU not specified"
  echo "Usage: ./tools/build.sh stm32h750 | stm32f103"
  exit 1
fi

# Project root = parent of tools/
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

BUILD_DIR="${ROOT_DIR}/build_${MCU}"
TOOLCHAIN_FILE="${ROOT_DIR}/cmake/toolchains/arm-gcc.cmake"

echo "============================================"
echo " OSAL Build System"
echo " Target MCU: ${MCU}"
echo " Root dir: ${ROOT_DIR}"
echo " Build dir: ${BUILD_DIR}"
echo "============================================"
echo ""

echo "[1/2] Configuring CMake..."
cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" \
  -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}" \
  -DMCU="${MCU}" \
  -GNinja || {
    echo "ERROR: CMake configuration failed"
    exit 1
  }

echo ""
echo "[2/2] Building firmware..."
cmake --build "${BUILD_DIR}" -j || {
    echo "ERROR: Build failed"
    exit 1
  }

echo ""
echo "============================================"
echo " Build Complete!"
echo " Output: ${BUILD_DIR}/app.{elf,hex,bin}"
echo "============================================"
