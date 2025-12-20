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
echo " Building for MCU: ${MCU}"
echo " Root dir: ${ROOT_DIR}"
echo " Build dir: ${BUILD_DIR}"
echo "============================================"

cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" \
  -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}" \
  -DMCU="${MCU}"

cmake --build "${BUILD_DIR}" -j
