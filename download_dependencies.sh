#!/usr/bin/env bash

# ---------------------------------------------------------------------------
# Download dependencies — downloads C++ dependencies.
# Assumes uv is available on PATH.
# Run from anywhere; paths are resolved relative to this script's location.
# ---------------------------------------------------------------------------

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PYTHON_DIR="$REPO_ROOT/python"

echo "Downloading C++ dependencies..."
cd "$PYTHON_DIR" || exit 1

# Export the variable so the python script can read it as an environment variable
export BASE_DIRECTORY="$REPO_ROOT"

uv run download-dependencies
if [ $? -ne 0 ]; then
    echo "[ERROR] download-dependencies failed."
    exit $?
fi

