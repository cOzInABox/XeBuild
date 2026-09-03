@echo off
setlocal
set version=17511
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

rem  batch        version   basepatch     outpatch          modedef
REM first the jtag machines
set jasper=1
set falcon=1
set xenon=1
set zephyr=1
call ..\bin\zdobuild.bat %version% %basepatch% out\patches_ "--defsym VFUSE=1 --defsym _PEEK_POKE=1"
REM then all the glitch machines
set xenon=
set trinity=1
set corona=1
set glitchpatch=1
call ..\bin\zdobuild.bat %version% %basepatch%g2 out\patches_g2 "--defsym GLITCH=1 --defsym _PEEK_POKE=1"
REM then the slim glitch2
set corona=
set trinity=
set jasper=
set falcon=
set zephyr=
set slim=1
call ..\bin\zdobuild.bat %version% %basepatch% out\patches_ "--defsym GLITCH=1 --defsym _PEEK_POKE=1"
REM then the fat glitch2
set slim=
set fat=1
call ..\bin\zdobuild.bat %version% %basepatch% out\patches_ "--defsym GLITCH=1 --defsym FATGLITCH=1 --defsym _PEEK_POKE=1"
REM finally the mfg glitch2
set trinity=1
set corona=1
REM set slim=1
set fat=
set glitchpatch=
set gltichmfgpatch=1
call ..\bin\zdobuild.bat %version% %basepatch%mfg out\patches_g2m "--defsym GLITCH=1 --defsym VFUSE=1 --defsym _PEEK_POKE=1"
set trinity=
set corona=1
call ..\bin\zdobuild.bat %version% %basepatch%mfg13182 out\patches_g2m13182 "--defsym GLITCH=1 --defsym VFUSE=1 --defsym _PEEK_POKE=1"
REM pause
endlocal

