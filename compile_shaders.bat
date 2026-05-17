@echo off
setlocal EnableDelayedExpansion

:: ---------------------------------------------------------------------------
:: Shader compilation script.
:: Run from anywhere; paths are resolved relative to this script's location.
:: ---------------------------------------------------------------------------

set "REPO_ROOT=%~dp0"
set "PYTHON_DIR=%REPO_ROOT%python"

:: BASE_DIRECTORY and TARGET_DIRECTORY are read by build_shaders.py via the environment
set "BASE_DIRECTORY=%REPO_ROOT%"
set "TARGET_DIRECTORY=%REPO_ROOT%assets\shaders"

:: ---- Check for uv ---------------------------------------------------------
where uv >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] uv not found on PATH.
    echo         Install it from: https://docs.astral.sh/uv/getting-started/installation/
    exit /b 1
)

:: ---- Compile shaders -------------------------------------------------------
echo Compiling shaders...
cd /d "%PYTHON_DIR%"
uv run build-shaders
if %errorlevel% neq 0 (
    echo [ERROR] build-shaders failed.
    goto end
)

echo.
echo Completed.

:end
choice /d n /t 1 /n /m "Press Y to pause"
if %errorlevel%==1 pause

endlocal