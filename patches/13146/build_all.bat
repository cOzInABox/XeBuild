@echo off
setlocal
set version=13146
set basepatch=base\patches_
IF NOT EXIST .\out\NUL mkdir out
rem  batch        version   basepatch     outpatch          modedef
echo ************** building internal NOMU patches **************
call ..\bin\zassembleopt.bat nointmu
rem call ..\bin\zdobuild.bat %version% %basepatch% out\KVHASH_patches_
set jasper=1
set falcon=1
set xenon=1
set zephyr=1
call ..\bin\zdobuild.bat %version% %basepatch% out\patches_ "--defsym NOKV_HASH=1"
endlocal

