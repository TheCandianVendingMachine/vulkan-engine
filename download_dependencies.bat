@echo off
setlocal EnableDelayedExpansion

:: ---------------------------------------------------------------------------
:: Download dependencies — downloads C++ dependencies.
:: Assumes uv is available on PATH.
:: Run from anywhere; paths are resolved relative to this script's location.
:: ---------------------------------------------------------------------------

set "REPO_ROOT=%~dp0"
set "PYTHON_DIR=%REPO_ROOT%python"

echo Downloading C++ dependencies...
cd /d "%PYTHON_DIR%"
set "BASE_DIRECTORY=%REPO_ROOT%"
uv run download-dependencies
if %errorlevel% neq 0 (
    echo [ERROR] download-dependencies failed.
    exit /b %errorlevel%
)

endlocal