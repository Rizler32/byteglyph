@echo off

"%~dp0bin/testbed.exe" %*

echo Exit code: %errorlevel%
if errorlevel 1 (
    exit /b %errorlevel%
)

exit /b 0
