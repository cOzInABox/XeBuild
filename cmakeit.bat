:: TODO: check if llvm-mingw produces working ARM binaries when cross-compiled.

@echo off
setlocal
:: my own machine has 8 cores/16 threads, setting this to 7 keeps
:: long running builds (this one isn't!) from bogging the machine down.
set NUM_THREADS=7
:: the directory cmake will use for the build
set CMAKE_BUILD_DIR=cbuild

:: since this is a .bat file, here are some windows build examples
:: uncomment *one*, correct the path if needed, and run the .bat file.

:: ---w64devkit---
:: https://github.com/skeeto/w64devkit/releases
:: cmake and ninja are included
:: set "PATH=C:\gccdev\w64devkit\bin"

:: ---llvm-mingw---
:: https://github.com/mstorsjo/llvm-mingw/releases
:: cmake isn't included
:: https://cmake.org
:: add ninja standalone binary to llvm-mingw\bin
:: https://ninja-build.org/
set "PATH=C:\gccdev\llvm-mingw\bin;C:\Program Files\CMake\bin"

:: ---msys2--- use appropriate toolchain
:: https://www.msys2.org/
:: set "PATH=H:\msys64\ucrt64\bin;H:\msys64\usr\bin"

:: ---visual studio 2026 community---
:: https://TODO
:: call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"

:: ---visual studio 2022 community---
:: https://TODO
:: call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

gcc -v
echo.
echo.
if %ERRORLEVEL% NEQ 0 (
    call showError "compiler not found on given path"
    exit /d 1
)

:: configure
cmake -S . -B %BUILD_DIR% -G Ninja -DCMAKE_MAKE_PROGRAM=ninja -DCMAKE_BUILD_TYPE=release
if %ERRORLEVEL% NEQ 0 (
    call showError "configure did not complete correctly"
    exit /d 1
)
echo.
echo.
echo Configuration complete.

echo.
echo.
echo Calling build...
:: build
cmake --build %BUILD_DIR% --config release -j %NUM_THREADS%
if %ERRORLEVEL% NEQ 0 (
    call showError "build did not complete correctly"
    exit /d 1
)
endlocal

:: copy to the places we want it...
del .\_build\xeBuild.exe .\x64\Debug\xeBuild.exe  >nul 2>&1
copy /b .\%BUILD_DIR%\source\xeBuild.exe .\testing\xeBuild.exe
copy /b .\%BUILD_DIR%\source\xeBuild.exe .\_build\xeBuild.exe

pause
exit /d 0

:showError
echo.
echo.
echo [ERROR] %~1.
pause
