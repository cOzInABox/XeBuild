@echo off
setlocal
:: this is an example of building with the w64devkit toolchain
:: cmake and ninja are included
:: set PATH=C:\gccdev\w64devkit\bin

:: this example shows how to build with the llvm-mingw toolchains
:: cmake isn't included and ninja has to be added from public releases
:: to llvm-mingw\bin
:: set PATH=C:\gccdev\llvm-mingw\busybox\bin;C:\gccdev\llvm-mingw\bin;C:\Program Files\CMake\bin
set PATH=C:\gccdev\llvm-mingw\bin;C:\Program Files\CMake\bin

:: also possible to build with msys2 install
:: set PATH=H:\msys64\ucrt64\bin;H:\msys64\usr\bin

:: clean
cmake --build cbuild --target clean

endlocal

:: copy to the places we want it...
REM del .\_build\xeBuild.exe .\x64\Debug\xeBuild.exe
REM copy /b .\xeBuild.exe .\x64\Debug\xeBuild.exe
REM copy /b .\xeBuild.exe .\_build\xeBuild.exe

pause
