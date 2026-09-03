:: builds everything, then copies the result to the
:: xebuild/_build folder for release

@echo off

call dobuild.bat 9199
call dobuild.bat 12611
call dobuild.bat 12625
call dobuild.bat 13146
call dobuild.bat 13599
call dobuild.bat 13604
call dobuild.bat 14699
call dobuild.bat 14717
call dobuild.bat 14719
call dobuild.bat 15574
call dobuild.bat 16197
call dobuild.bat 16202
call dobuild.bat 16203
call dobuild.bat 16537
call dobuild.bat 16547
call dobuild.bat 16747
call dobuild.bat 16756
call dobuild.bat 16767
call dobuild.bat 17148
call dobuild.bat 17150
call dobuild.bat 17349
call dobuild.bat 17489
call dobuild.bat 17502
call dobuild.bat 17511
call dobuild.bat 17526
call dobuild.bat 17544
call dobuild.bat 17559

pause
