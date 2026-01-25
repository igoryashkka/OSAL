#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

echo "============================================"
echo " OSAL Clean - Removing Build Artifacts"
echo " Root dir: ${ROOT_DIR}"
echo "============================================"
echo ""

# Remove build directories
echo "Cleaning build directories..."
rm -rf "${ROOT_DIR}/build_"*
rm -rf "${ROOT_DIR}/build"
rm -rf "${ROOT_DIR}/cmake-build-"*

# Remove CMake cache files in root
echo "Cleaning CMake cache..."
rm -rf "${ROOT_DIR}/CMakeFiles"
rm -f  "${ROOT_DIR}/CMakeCache.txt"
rm -f  "${ROOT_DIR}/cmake_install.cmake"
rm -f  "${ROOT_DIR}/Makefile"

echo ""
echo "============================================"
echo " Clean Complete!"
echo "============================================"

rm -f  "${ROOT_DIR}/"*.elf "${ROOT_DIR}/"*.hex "${ROOT_DIR}/"*.bin "${ROOT_DIR}/"*.map

echo "Done."
