@echo off

set BASE_DIRECTORY=%cd%
set TARGET_DIRECTORY=%BASE_DIRECTORY%/assets/shaders

cd python
uv run tools/build_shaders.py

echo ...
echo Completed
choice /d n /t 1 /n /m "Press Y to Pause"
if %ERRORLEVEL%==1 pause
