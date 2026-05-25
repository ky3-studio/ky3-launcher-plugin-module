@echo off
setlocal

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1

set SRC_DIR=%~dp0
set SRC_DIR=%SRC_DIR:~0,-1%
set OUT_DIR=D:\kyxsan-launcher\kyxsan-zr\bin\Release
set OUT_NAME=Ky3-luancher-Plugin

if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"

echo [BUILD] Compiling...

pushd "%TEMP%"

cl /nologo /O2 /MD /DNDEBUG /DWIN32 /D_WINDOWS /D_USRDLL /EHsc /W0 /LD /I"%SRC_DIR%" /Fe"%OUT_DIR%\%OUT_NAME%.dll" "%SRC_DIR%\dllmain.cpp" "%SRC_DIR%\MinHook\buffer.c" "%SRC_DIR%\MinHook\hook.c" "%SRC_DIR%\MinHook\trampoline.c" "%SRC_DIR%\MinHook\hde32.c" "%SRC_DIR%\MinHook\hde64.c" /link /MACHINE:X64 /OPT:REF /OPT:ICF /IMPLIB:"%TEMP%\%OUT_NAME%.lib" kernel32.lib user32.lib ws2_32.lib psapi.lib dwmapi.lib gdi32.lib advapi32.lib

set BUILD_ERR=%errorlevel%

del /q *.obj 2>nul
del /q "%OUT_NAME%.lib" 2>nul
del /q "%OUT_NAME%.exp" 2>nul

popd

if %BUILD_ERR% neq 0 (
    echo [BUILD] FAILED
    exit /b 1
)

echo [BUILD] Success: %OUT_DIR%\%OUT_NAME%.dll
exit /b 0
