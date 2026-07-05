#!/usr/bin/env bash

# ---------------------------------------------------------------------------
# Setup script — installs Python deps, downloads C++ dependencies,
# and compiles shaders.
# Run from anywhere; paths are resolved relative to this script's location.
# ---------------------------------------------------------------------------

# Get the directory where this script is located
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# ---- Check for uv ---------------------------------------------------------
if ! command -v uv &> /dev/null; then
    echo "[ERROR] uv not found on PATH."
    echo "        Install it from: https://docs.astral.sh/uv/getting-started/installation/"
    exit 1
fi

# ---- Sync Python environment ----------------------------------------------
echo "[1/3] Syncing Python environment..."
cd "$REPO_ROOT/python" || exit 1

uv sync
if [ $? -ne 0 ]; then
    echo "[ERROR] uv sync failed."
    exit $?
fi

# ---- Download dependencies ------------------------------------------------
echo ""
echo "[2/3] Downloading dependencies..."
echo ""
# Assumes the batch file was converted to a shell script (.sh)
if [ -f "$REPO_ROOT/download_dependencies.sh" ]; then
    source "$REPO_ROOT/download_dependencies.sh"
else
    echo "[ERROR] download_dependencies.sh not found."
    exit 1
fi
if [ $? -ne 0 ]; then exit $?; fi

# ---- Compile shaders -------------------------------------------------------
echo ""
echo "[3/3] Compiling shaders..."
echo ""
# Assumes the batch file was converted to a shell script (.sh)
if [ -f "$REPO_ROOT/compile_shaders.sh" ]; then
    source "$REPO_ROOT/compile_shaders.sh"
else
    echo "[ERROR] compile_shaders.sh not found."
    exit 1
fi
if [ $? -ne 0 ]; then exit $?; fi

echo ""
echo "Done. You can now configure and build with CMake:"
echo "   cmake --preset clang"
echo "   cmake --build --preset clang-release"

