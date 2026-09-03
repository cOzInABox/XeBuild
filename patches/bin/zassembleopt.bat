@echo off
setlocal
set srcname=%1
set opts=%~2


if not exist %srcname%.S goto NOSOURCE
..\bin\xenon-as.exe -be -many -mregnames %opts% %srcname%.S -o %srcname%.elf
IF %ERRORLEVEL% NEQ 0 goto SHOWERROR
if not exist %srcname%.elf goto NOELF

..\bin\xenon-objcopy.exe %srcname%.elf -O binary opt\%srcname%.bin
IF %ERRORLEVEL% NEQ 0 goto SHOWERROR
del /q %srcname%.elf
if not exist opt\%srcname%.bin goto NOBIN
goto EXIT

:NOSOURCE
echo.
echo source file missing, cannot proceed
goto BADEXIT

:NOELF
echo.
echo elf did not assemble, cannot proceed
goto BADEXIT

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

:EXIT
endlocal
