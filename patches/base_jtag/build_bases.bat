@echo off
setlocal

set onebl=ca_1BL_all
set xenoncb=cb_1940_xenon
set zephyrcb=cb_4579_zephyr
set falconcb=cb_5771_falcon
set jaspercb=cb_6750_jasper
set cdbase=cd_8453
set onecd=%cdbase%_all.S

rem Some cleanup
..\bin\rm -f bin\*.* *.elf *.bin

echo assembling 1BL patches
call ..\bin\zassemble.bat %onebl%
echo assembling xenon cb patches
call ..\bin\zassemble.bat %xenoncb%
echo assembling zephyr cb patches
call ..\bin\zassemble.bat %zephyrcb%
echo assembling falcon cb patches
call ..\bin\zassemble.bat %falconcb%
echo assembling jasper cb patches
call ..\bin\zassemble.bat %jaspercb%

echo assembling xenon CD patches
set tail=_xenon
..\bin\xenon-as.exe -be -many --defsym XENON=1 %onecd% -o %cdbase%%tail%.elf
IF %ERRORLEVEL% NEQ 0 goto SHOWERROR
..\bin\xenon-objcopy.exe %cdbase%%tail%.elf -O binary %cdbase%%tail%.bin
IF %ERRORLEVEL% NEQ 0 goto SHOWERROR
del /q %cdbase%%tail%.elf
if not exist %cdbase%%tail%.bin goto NOBIN

echo assembling zephyr CD patches
set tail=_zephyr
..\bin\xenon-as.exe -be -many --defsym ZEPHYR=1 %onecd% -o %cdbase%%tail%.elf
IF %ERRORLEVEL% NEQ 0 goto SHOWERROR
..\bin\xenon-objcopy.exe %cdbase%%tail%.elf -O binary %cdbase%%tail%.bin
IF %ERRORLEVEL% NEQ 0 goto SHOWERROR
del /q %cdbase%%tail%.elf
if not exist %cdbase%%tail%.bin goto NOBIN

echo assembling falcon CD patches
set tail=_falcon
..\bin\xenon-as.exe -be -many --defsym FALCON=1 %onecd% -o %cdbase%%tail%.elf
IF %ERRORLEVEL% NEQ 0 goto SHOWERROR
..\bin\xenon-objcopy.exe %cdbase%%tail%.elf -O binary %cdbase%%tail%.bin
IF %ERRORLEVEL% NEQ 0 goto SHOWERROR
del /q %cdbase%%tail%.elf
if not exist %cdbase%%tail%.bin goto NOBIN

echo assembling jasper CD patches
set tail=_jasper
..\bin\xenon-as.exe -be -many --defsym JASPER=1 %onecd%  -o %cdbase%%tail%.elf
IF %ERRORLEVEL% NEQ 0 goto SHOWERROR
..\bin\xenon-objcopy.exe %cdbase%%tail%.elf -O binary %cdbase%%tail%.bin
IF %ERRORLEVEL% NEQ 0 goto SHOWERROR
del /q %cdbase%%tail%.elf
if not exist %cdbase%%tail%.bin goto NOBIN


echo.

echo creating xenon base patches
copy /b %onebl%.bin+%xenoncb%.bin+%cdbase%_xenon.bin .\bin\patches_xenon_base.bin
echo creating zephyr base patches
copy /b %onebl%.bin+%zephyrcb%.bin+%cdbase%_zephyr.bin .\bin\patches_zephyr_base.bin
echo creating falcon base patches
copy /b %onebl%.bin+%falconcb%.bin+%cdbase%_falcon.bin .\bin\patches_falcon_base.bin
echo creating jasper base patches
copy /b %onebl%.bin+%jaspercb%.bin+%cdbase%_jasper.bin .\bin\patches_jasper_base.bin
echo.

echo ** SUCCESS! Base Patches Built **
echo.
..\bin\rm -f *.bin


:EXIT
endlocal
pause
exit

:NOBIN
echo.
echo bin file missing, cannot proceed
goto BADEXIT

:SHOWERROR
echo Application returned error status of %ERRORLEVEL%
echo Abnormal script termination!

:BADEXIT
endlocal
pause
exit

