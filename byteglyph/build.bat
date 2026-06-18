@echo off
SetLocal EnableDelayedExpansion

set cfiles=
for /R "%~dp0src" %%f in (*.c) do (
    set cfiles=!cfiles! "%%f"
)

set asm=byteglyph
set compilerFlags=-Wall -Werror
set libInclude=-I"C:/Dev/C++/lib/atil-1.0.0/include"
set includeFlags=-I"%~dp0include" -I"%~dp0src"
set defines=-D_TRACE

echo Building %asm%
set "bindir=%~dp0..\bin"
if not exist "%bindir%" mkdir "%bindir%"

rem --- COMPILA I .c IN .o ---
set ofiles=
for %%f in (!cfiles!) do (
    set "ofile=%%~nf.o"
    gcc -c "%%~f" -o "!ofile!" %compilerFlags% %libInclude% %defines% %includeFlags%
    if errorlevel 1 (
        echo Compilation failed for %%~f
        exit /b %errorlevel%
    )
    set ofiles=!ofiles! "!ofile!"
)

rem --- CREA LA LIBRERIA STATICA ---
ar rcs "%bindir%\lib%asm%.a" %ofiles%

if errorlevel 0 (
    echo Built successfully
) else (
    echo Built with %errorlevel% exit code
)

rem --- PULIZIA ---
del *.o
