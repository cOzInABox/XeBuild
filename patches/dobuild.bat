:: This file is not meant to be run directly!!

@echo off
setlocal

set base=%1

echo.
echo  ********************
echo *** building %base% ***
echo  ********************
echo.
cd %base%
call build_all.bat
rem for build folder
echo copying results .\out\*.bin to ..\..\_build\%base%\bin\*.bin
if exist ..\..\_build\%base%\bin\ goto EXIST1
mkdir ..\..\_build\%base%
mkdir ..\..\_build\%base%\bin
:EXIST1
if exist .\out\patches_xenon.bin copy /b .\out\patches_xenon.bin ..\..\_build\%base%\bin\patches_xenon.bin
if exist .\out\patches_zephyr.bin copy /b .\out\patches_zephyr.bin ..\..\_build\%base%\bin\patches_zephyr.bin
if exist .\out\patches_falcon.bin copy /b .\out\patches_falcon.bin ..\..\_build\%base%\bin\patches_falcon.bin
if exist .\out\patches_jasper.bin copy /b .\out\patches_jasper.bin ..\..\_build\%base%\bin\patches_jasper.bin
if exist .\out\patches_g2xenon.bin copy /b .\out\patches_g2xenon.bin ..\..\_build\%base%\bin\patches_g2xenon.bin
if exist .\out\patches_g2zephyr.bin copy /b .\out\patches_g2zephyr.bin ..\..\_build\%base%\bin\patches_g2zephyr.bin
if exist .\out\patches_g2jasper.bin copy /b .\out\patches_g2jasper.bin ..\..\_build\%base%\bin\patches_g2jasper.bin
if exist .\out\patches_g2falcon.bin copy /b .\out\patches_g2falcon.bin ..\..\_build\%base%\bin\patches_g2falcon.bin
if exist .\out\patches_g2trinity.bin copy /b .\out\patches_g2trinity.bin ..\..\_build\%base%\bin\patches_g2trinity.bin
if exist .\out\patches_g2corona.bin copy /b .\out\patches_g2corona.bin ..\..\_build\%base%\bin\patches_g2corona.bin
if exist .\out\patches_g2mtrinity.bin copy /b .\out\patches_g2mtrinity.bin ..\..\_build\%base%\bin\patches_g2mtrinity.bin
if exist .\out\patches_g2mcorona.bin copy /b .\out\patches_g2mcorona.bin ..\..\_build\%base%\bin\patches_g2mcorona.bin
if exist .\out\patches_g2mWBcorona.bin copy /b .\out\patches_g2mWBcorona.bin ..\..\_build\%base%\bin\patches_g2mcorona_WB.bin
if exist .\out\patches_g2mWB4Gcorona.bin copy /b .\out\patches_g2mWB4Gcorona.bin ..\..\_build\%base%\bin\patches_g2mcorona_WB4G.bin
if exist .\out\patches_g2WBcorona.bin copy /b .\out\patches_g2WBcorona.bin ..\..\_build\%base%\bin\patches_g2corona_WB.bin
if exist .\out\patches_g2WB4Gcorona.bin copy /b .\out\patches_g2WB4Gcorona.bin ..\..\_build\%base%\bin\patches_g2corona_WB4G.bin

if exist .\out\patches_fat.bin copy /b .\out\patches_fat.bin ..\..\_build\%base%\bin\patches_fat.bin
if exist .\out\patches_trinity.bin copy /b .\out\patches_trinity.bin ..\..\_build\%base%\bin\patches_trinity.bin
copy /b .\opt\*.bin ..\..\_build\%base%\bin\

rem for debug folder
echo copying results .\out\*.bin to ..\..\Debug\%base%\bin\*.bin
if exist ..\..\Debug\%base%\bin\ goto EXIST2
mkdir ..\..\Debug\%base%
mkdir ..\..\Debug\%base%\bin
:EXIST2
if exist .\out\patches_xenon.bin copy /b .\out\patches_xenon.bin ..\..\Debug\%base%\bin\patches_xenon.bin
if exist .\out\patches_zephyr.bin copy /b .\out\patches_zephyr.bin ..\..\Debug\%base%\bin\patches_zephyr.bin
if exist .\out\patches_falcon.bin copy /b .\out\patches_falcon.bin ..\..\Debug\%base%\bin\patches_falcon.bin
if exist .\out\patches_jasper.bin copy /b .\out\patches_jasper.bin ..\..\Debug\%base%\bin\patches_jasper.bin
if exist .\out\patches_g2xenon.bin copy /b .\out\patches_g2xenon.bin ..\..\Debug\%base%\bin\patches_g2xenon.bin
if exist .\out\patches_g2zephyr.bin copy /b .\out\patches_g2zephyr.bin ..\..\Debug\%base%\bin\patches_g2zephyr.bin
if exist .\out\patches_g2jasper.bin copy /b .\out\patches_g2jasper.bin ..\..\Debug\%base%\bin\patches_g2jasper.bin
if exist .\out\patches_g2falcon.bin copy /b .\out\patches_g2falcon.bin ..\..\Debug\%base%\bin\patches_g2falcon.bin
if exist .\out\patches_g2trinity.bin copy /b .\out\patches_g2trinity.bin ..\..\Debug\%base%\bin\patches_g2trinity.bin
if exist .\out\patches_g2corona.bin copy /b .\out\patches_g2corona.bin ..\..\Debug\%base%\bin\patches_g2corona.bin
if exist .\out\patches_g2mtrinity.bin copy /b .\out\patches_g2mtrinity.bin ..\..\Debug\%base%\bin\patches_g2mtrinity.bin
if exist .\out\patches_g2mcorona.bin copy /b .\out\patches_g2mcorona.bin ..\..\Debug\%base%\bin\patches_g2mcorona.bin
if exist .\out\patches_g2mWBcorona.bin copy /b .\out\patches_g2mWBcorona.bin ..\..\Debug\%base%\bin\patches_g2mcorona_WB.bin
if exist .\out\patches_g2mWB4Gcorona.bin copy /b .\out\patches_g2mWB4Gcorona.bin ..\..\Debug\%base%\bin\patches_g2mcorona_WB4G.bin
if exist .\out\patches_g2WBcorona.bin copy /b .\out\patches_g2WBcorona.bin ..\..\Debug\%base%\bin\patches_g2corona_WB.bin
if exist .\out\patches_g2WB4Gcorona.bin copy /b .\out\patches_g2WB4Gcorona.bin ..\..\Debug\%base%\bin\patches_g2corona_WB4G.bin
if exist .\out\patches_fat.bin copy /b .\out\patches_fat.bin ..\..\Debug\%base%\bin\patches_fat.bin
if exist .\out\patches_trinity.bin copy /b .\out\patches_trinity.bin ..\..\Debug\%base%\bin\patches_trinity.bin
copy /b .\opt\*.bin ..\..\Debug\%base%\bin\

rem for dashlaunch folder
echo copying results .\out\*.bin to ..\_dl\parc_%base%.bin
..\bin\dl_patches.exe .\out\ ..\_dl\parc_%base%.bin
echo creating update idx for addons
..\bin\opt_idx.exe
copy /b .\opt\*.idx ..\..\_build\%base%\bin\
copy /b .\opt\*.idx ..\..\Debug\%base%\bin\

cd ..
echo done!

endlocal
