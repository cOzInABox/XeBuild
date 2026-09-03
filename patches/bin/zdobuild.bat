@echo off
setlocal
set basename=patches_kernel_%1
set basepatch=%2
set outpatch=%3
set modedef=%~4

REM echo basename : %basename%
REM echo basepatch: %basepatch%
REM echo outpatch : %outpatch%
REM echo modedef  : %modedef%

rem Some cleanup
..\bin\rm -f %outpatch%*.* %basename%.elf %basename%.bin
if not exist %basename%.S goto NOFILE1

echo Calling assembler
..\bin\xenon-as.exe -be -many -mregnames %modedef% %basename%.S -o %basename%.elf
IF %ERRORLEVEL% NEQ 0 goto SHOWERROR

if not exist %basename%.elf goto NOFILE2
echo Calling objcopy
..\bin\xenon-objcopy.exe %basename%.elf -O binary %basename%.bin
IF %ERRORLEVEL% NEQ 0 goto SHOWERROR
del /q %basename%.elf

if not exist %basename%.bin goto NOFILE3
echo creating patch files in directory "out"

:FALCON
if not defined falcon goto JASPER
copy /b %basepatch%falcon_base.bin+%basename%.bin /b %outpatch%falcon.bin
if defined glitchpatch copy /b ..\bin\dummybytes.bin+%basename%.bin /b %outpatch%falcon_dl.bin
if defined gltichmfgpatch copy /b %basename%.bin /b %outpatch%falcon_dl.bin
if not exist %outpatch%falcon.bin goto NOPATCHES

:JASPER
if not defined jasper goto XENON
copy /b %basepatch%jasper_base.bin+%basename%.bin /b %outpatch%jasper.bin
if defined glitchpatch copy /b ..\bin\dummybytes.bin+%basename%.bin /b %outpatch%jasper_dl.bin
if defined gltichmfgpatch copy /b %basename%.bin /b %outpatch%jasper_dl.bin
if not exist %outpatch%jasper.bin goto NOPATCHES

:XENON
if not defined xenon goto ZEPHYR
copy /b %basepatch%xenon_base.bin+%basename%.bin /b %outpatch%xenon.bin
if defined glitchpatch copy /b ..\bin\dummybytes.bin+%basename%.bin /b %outpatch%xenon_dl.bin
if defined gltichmfgpatch copy /b %basename%.bin /b %outpatch%xenon_dl.bin
if not exist %outpatch%xenon.bin goto NOPATCHES

:ZEPHYR
if not defined zephyr goto TRINITY
copy /b %basepatch%zephyr_base.bin+%basename%.bin /b %outpatch%zephyr.bin
if defined glitchpatch copy /b ..\bin\dummybytes.bin+%basename%.bin /b %outpatch%zephyr_dl.bin
if defined gltichmfgpatch copy /b %basename%.bin /b %outpatch%zephyr_dl.bin
if not exist %outpatch%zephyr.bin goto NOPATCHES

:TRINITY
if not defined trinity goto NOTRINITY
copy /b %basepatch%trinity_base.bin+%basename%.bin /b %outpatch%trinity.bin
if defined glitchpatch copy /b ..\bin\dummybytes.bin+%basename%.bin /b %outpatch%trinity_dl.bin
if defined gltichmfgpatch copy /b %basename%.bin /b %outpatch%trinity_dl.bin
if not exist %outpatch%trinity.bin goto NOPATCHES

:NOTRINITY
if not defined corona goto NOCORONA
copy /b %basepatch%corona_base.bin+%basename%.bin /b %outpatch%corona.bin
if defined glitchpatch copy /b ..\bin\dummybytes.bin+%basename%.bin /b %outpatch%corona_dl.bin
if defined gltichmfgpatch copy /b %basename%.bin /b %outpatch%corona_dl.bin
if not exist %outpatch%corona.bin goto NOPATCHES

:NOCORONA
if not defined slim goto NOSLIMGLITCH
copy /b %basepatch%trinity_base.bin+%basename%.bin /b %outpatch%trinity.bin
if defined glitchpatch copy /b ..\bin\dummybytes.bin+%basename%.bin /b %outpatch%trinity_dl.bin
if not exist %outpatch%trinity.bin goto NOPATCHES

:NOSLIMGLITCH
if not defined fat goto NOFATGLITCH
copy /b %basepatch%fat_base.bin+%basename%.bin /b %outpatch%fat.bin
if defined glitchpatch copy /b ..\bin\dummybytes.bin+%basename%.bin /b %outpatch%fat_dl.bin
if not exist %outpatch%trinity.bin goto NOPATCHES

:NOFATGLITCH

..\bin\rm -f %basename%.elf %basename%.bin

echo ** SUCCESS! built %outpatch%*.bin **
echo.
goto EXIT

:NOFILE1
echo.
echo %basename%.S missing, cannot proceed
goto EXIT

:NOFILE2
echo.
echo %basename%.elf did not assemble, cannot proceed
goto EXIT

:NOFILE3
echo.
echo %basename%.bin did not build, cannot proceed
goto EXIT

:NOPATCHES
..\bin\rm -f %basename%.elf %basename%.bin
echo.
echo %outpatch% is not in the "out" directory
goto EXIT

:SHOWERROR
echo Application returned error status of %ERRORLEVEL%
echo Abnormal script termination!
endlocal
pause
exit

:EXIT
REM pause
endlocal


