#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

echo "============================================"
echo " Cleaning ALL build artifacts"
echo " Root dir: ${ROOT_DIR}"
echo "============================================"

rm -rf "${ROOT_DIR}/build_"*
rm -rf "${ROOT_DIR}/build"
rm -rf "${ROOT_DIR}/cmake-build-"*

rm -rf "${ROOT_DIR}/CMakeFiles"
rm -f  "${ROOT_DIR}/CMakeCache.txt"
rm -f  "${ROOT_DIR}/cmake_install.cmake"
rm -f  "${ROOT_DIR}/Makefile"

rm -f  "${ROOT_DIR}/"*.elf "${ROOT_DIR}/"*.hex "${ROOT_DIR}/"*.bin "${ROOT_DIR}/"*.map

echo "Done."
