@echo off
setlocal
set version=16197
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
set jasper=1
rem call ..\bin\zdobuild.bat %version% %basepatch% out\KVHASH_patches_
set falcon=1
set xenon=1
set zephyr=1
call ..\bin\zdobuild.bat %version% %basepatch% out\patches_
set xenon=
set trinity=1
set corona=1
set glitchpatch=1
call ..\bin\zdobuild.bat %version% %basepatch%g2 out\patches_g2 "--defsym GLITCH=1"
set corona=
set trinity=
set jasper=
set falcon=
set zephyr=
set slim=1
call ..\bin\zdobuild.bat %version% %basepatch% out\patches_ "--defsym GLITCH=1"
set slim=
set fat=1
call ..\bin\zdobuild.bat %version% %basepatch% out\patches_ "--defsym GLITCH=1 --defsym FATGLITCH=1"

endlocal

