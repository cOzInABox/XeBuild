@echo off
setlocal
set version=17526
set basepatch=base\patches_
IF NOT EXIST .\out\NUL mkdir out

echo ************** building internal NOMU patches **************
call ..\bin\zassembleopt.bat nointmu
echo ************** building NOHDD patches **************
call ..\bin\zassembleopt.bat nohdd
echo ************** building NOFCRT patches **************
call ..\bin\zassembleopt.bat nofcrt
echo ************** building NOHDMIWAIT patches **************
call ..\bin\zassembleopt.bat nohdmiwait
echo ************** building NOLAN patches **************
call ..\bin\zassembleopt.bat nolan
echo ************** building noSShdd patches **************
call ..\bin\zassembleopt.bat noSShdd
echo ************** building NoWifi patches **************
call ..\bin\zassembleopt.bat nowifi

REM echo ************** building xferCablePatch patches **************
REM call ..\bin\zassembleopt.bat xferCablePatch

rem  batch        version   basepatch     outpatch          modedef
REM first the jtag machines
set jasper=1
set falcon=1
set xenon=1
set zephyr=1
call ..\bin\zdobuild.bat %version% %basepatch% out\patches_ "--defsym VFUSE=1"
REM then all the glitch machines
set xenon=
set trinity=1
set corona=1
set glitchpatch=1
call ..\bin\zdobuild.bat %version% %basepatch%g2 out\patches_g2 "--defsym GLITCH=1"
REM then the slim glitch2
set corona=
set trinity=
set jasper=
set falcon=
set zephyr=
set slim=1
call ..\bin\zdobuild.bat %version% %basepatch% out\patches_ "--defsym GLITCH=1"
REM then the fat glitch2
set slim=
set fat=1
call ..\bin\zdobuild.bat %version% %basepatch% out\patches_ "--defsym GLITCH=1 --defsym FATGLITCH=1"
REM finally the mfg glitch2
set trinity=1
set corona=1
REM set slim=1
set fat=
set glitchpatch=
set gltichmfgpatch=1
call ..\bin\zdobuild.bat %version% %basepatch%mfg out\patches_g2m "--defsym GLITCH=1 --defsym VFUSE=1"
REM alternate corona patches
set trinity=
set corona=1
call ..\bin\zdobuild.bat %version% %basepatch%mfgWB out\patches_g2mWB "--defsym GLITCH=1 --defsym VFUSE=1"
call ..\bin\zdobuild.bat %version% %basepatch%mfgWB4G out\patches_g2mWB4G "--defsym GLITCH=1 --defsym VFUSE=1"
call ..\bin\zdobuild.bat %version% %basepatch%g2WB out\patches_g2WB "--defsym GLITCH=1"
call ..\bin\zdobuild.bat %version% %basepatch%g2WB4G out\patches_g2WB4G "--defsym GLITCH=1"

pause
endlocal

