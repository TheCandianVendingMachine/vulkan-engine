#!/usr/bin/env bash

# ---------------------------------------------------------------------------
# Shader compilation script.
# Assumes uv is available on PATH.
# Run from anywhere; paths are resolved relative to this script's location.
# ---------------------------------------------------------------------------

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PYTHON_DIR="$REPO_ROOT/python"

# Export variables so uv run can see them, and use forward slashes for paths
export BASE_DIRECTORY="$REPO_ROOT"
export TARGET_DIRECTORY="$REPO_ROOT/assets/shaders"

echo "Compiling shaders..."
cd "$PYTHON_DIR" || exit 1

uv run build-shaders
if [ $? -ne 0 ]; then
    echo "[ERROR] build-shaders failed."
    exit $?
fi

echo ""
echo "Completed."

