#!/usr/bin/env bash
# Build CurveFit for macOS ARM64 (M1-M3)
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
INSTALL_DIR="${SCRIPT_DIR}/install"

echo "=== Installing dependencies via Homebrew ==="
brew install cmake ninja eigen ceres-solver qt@6

echo "=== Configuring ==="
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

cmake -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -DCMAKE_PREFIX_PATH="$(brew --prefix qt@6)" \
    -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" \
    "${SCRIPT_DIR}"

echo "=== Building ==="
cmake --build . --parallel

echo "=== Installing ==="
cmake --install .

echo "=== Done ==="
echo "App bundle: ${INSTALL_DIR}/CurveFitScientificApp.app"
echo "To create DMG: ./package_mac.sh ${INSTALL_DIR}/CurveFitScientificApp.app"
