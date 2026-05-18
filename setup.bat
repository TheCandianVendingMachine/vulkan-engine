@echo off
setlocal EnableDelayedExpansion

:: ---------------------------------------------------------------------------
:: Setup script — installs Python deps, downloads C++ dependencies,
:: and compiles shaders.
:: Run from anywhere; paths are resolved relative to this script's location.
:: ---------------------------------------------------------------------------

set "REPO_ROOT=%~dp0"

:: ---- Check for uv ---------------------------------------------------------
where uv >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] uv not found on PATH.
    echo         Install it from: https://docs.astral.sh/uv/getting-started/installation/
    exit /b 1
)

:: ---- Sync Python environment ----------------------------------------------
echo [1/3] Syncing Python environment...
cd /d "%REPO_ROOT%python"
uv sync
if %errorlevel% neq 0 (
    echo [ERROR] uv sync failed.
    exit /b %errorlevel%
)

:: ---- Download dependencies ------------------------------------------------
echo.
echo [2/3] Downloading dependencies...
echo.
call "%REPO_ROOT%download_dependencies.bat"
if %errorlevel% neq 0 exit /b %errorlevel%

:: ---- Compile shaders -------------------------------------------------------
echo.
echo [3/3] Compiling shaders...
echo.
call "%REPO_ROOT%compile_shaders.bat"
if %errorlevel% neq 0 exit /b %errorlevel%

echo.
echo Done. You can now configure and build with CMake:
echo   cmake --preset clang
echo   cmake --build --preset clang-release

endlocal