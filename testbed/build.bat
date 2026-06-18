@echo off
setlocal enabledelayedexpansion

pushd "%~dp0" >nul 2>&1 || exit /b 1

set "platform_specific_libs="
set "compilerFlags=-lbyteglyph -latil_util -latil_log -latil_sys"
set "defines=-D_TRACE"

rem pass all defines as arguments given to this script
for %%d in (%*) do (
    set defines=!defines! -D%%d
)

set "includeDir=%~dp0include"
set "srcDir=%~dp0src"
set "libIncludeDir=C:/Dev/C++/lib/atil-1.0.0/include"
set "libDir=C:/Dev/C++/lib/atil-1.0.0/bin"
set "byteglyphIncludeDir=%~dp0../byteglyph/include"
set "byteglyphLibDir=%~dp0../bin"
set "bindir=%~dp0..\bin"
set "msysZstdInclude=C:/msys64/ucrt64/include"
set "msysZstdLib=C:/msys64/ucrt64/lib"
set "extraIncludeFlags="
set "extraLibDirFlags="
set "extraLibFlags="
if exist "%msysZstdInclude%\zstd.h" set "extraIncludeFlags=-I%msysZstdInclude%"
if exist "%msysZstdLib%\libzstd.a" (
    set "extraLibDirFlags=-L%msysZstdLib%"
    set "extraLibFlags=-lzstd"
)

if not exist "%bindir%" mkdir "%bindir%"

echo Building testbed

REM ---------------------------------------------------------
REM 1. Raccogli tutti i .c da src/ e include/
REM ---------------------------------------------------------
set "sources="

for /r "%srcDir%" %%f in (*.c) do (
    set "sources=!sources! "%%f""
)

REM Aggiungi main.c dell'app se esiste
if exist "main.c" (
    set "sources=!sources! main.c"
) else (
    echo WARNING: main.c not found in app directory.
)

REM ---------------------------------------------------------
REM 2. Compila tutto in un colpo solo
REM ---------------------------------------------------------
gcc %sources% ^
    %defines% ^
    %compilerFlags% ^
    %platform_specific_libs% ^
    %vulkanInclude% ^
    -I"%includeDir%" %extraIncludeFlags% -I"%libIncludeDir%" %extraLibDirFlags% -I"%srcDir%" -I"%byteglyphIncludeDir%" ^
    -L"%byteglyphLibDir%" -L"%libDir%" ^
    %extraLibFlags% ^
    -o "%bindir%\testbed.exe"

if errorlevel 1 (
    echo ERROR: compilation failed.
    exit /b 1
)

popd >nul 2>&1

echo Build complete.
