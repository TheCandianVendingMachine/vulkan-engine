@echo off
setlocal EnableDelayedExpansion

:: ---------------------------------------------------------------------------
:: Setup script — installs Python deps, downloads C++ dependencies,
:: and compiles shaders.
:: Run from anywhere; paths are resolved relative to this script's location.
:: ---------------------------------------------------------------------------

set "REPO_ROOT=%~dp0"
set "PYTHON_DIR=%REPO_ROOT%python"

:: ---- Check for uv ---------------------------------------------------------
where uv >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] uv not found on PATH.
    echo         Install it from: https://docs.astral.sh/uv/getting-started/installation/
    exit /b 1
)

:: ---- Sync Python environment ----------------------------------------------
echo.
echo [1/3] Syncing Python environment...
cd /d "%PYTHON_DIR%"
uv sync
if %errorlevel% neq 0 (
    echo [ERROR] uv sync failed.
    exit /b %errorlevel%
)

:: ---- Download C++ dependencies --------------------------------------------
echo.
echo [2/3] Downloading C++ dependencies...
set "BASE_DIRECTORY=%REPO_ROOT%"
uv run download-dependencies
if %errorlevel% neq 0 (
    echo [ERROR] download-dependencies failed.
    exit /b %errorlevel%
)

:: ---- Compile shaders -------------------------------------------------------
echo.
echo [3/3] Compiling shaders...
set "TARGET_DIRECTORY=%REPO_ROOT%assets\shaders"
uv run build-shaders
if %errorlevel% neq 0 (
    echo [ERROR] build-shaders failed.
    exit /b %errorlevel%
)

echo.
echo Done. You can now configure and build with CMake:
echo   cmake --preset msvc
echo   cmake --build --preset msvc-release

endlocal