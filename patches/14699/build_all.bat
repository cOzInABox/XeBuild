@echo off
setlocal
set version=14699
set basepatch=base\patches_
IF NOT EXIST .\out\NUL mkdir out

echo ************** building internal NOMU patches **************
call ..\bin\zassembleopt.bat nointmu
echo ************** building NOFCRT patches **************
call ..\bin\zassembleopt.bat nofcrt

rem  batch        version   basepatch     outpatch          modedef
set jasper=1
rem call ..\bin\zdobuild.bat %version% %basepatch% out\KVHASH_patches_
set falcon=1
set xenon=1
set zephyr=1
call ..\bin\zdobuild.bat %version% %basepatch% out\patches_ "--defsym NOKV_HASH=1"
set jasper=
set falcon=
set xenon=
set zephyr=
set slim=1
call ..\bin\zdobuild.bat %version% %basepatch% out\patches_ "--defsym GLITCH=1"
set slim=
set fat=1
call ..\bin\zdobuild.bat %version% %basepatch% out\patches_ "--defsym GLITCH=1 --defsym FATGLITCH=1"

endlocal

