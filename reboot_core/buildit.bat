@echo off
setlocal
set PATH=..\xenon\bin;%PATH%
set TMP=C:\TEMP
make
if exist freeboot.h copy /b freeboot.h ..\source\freeboot.h
if exist payload.h copy /b payload.h ..\source\payload.h

endlocal
pause