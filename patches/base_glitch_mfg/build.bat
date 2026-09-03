@echo off
setlocal
set BASE_SRC=base_build
set BASE_NME=base_trin.bin

rm -f *.bin

call ..\bin\zassemble.bat cb_b_9188_patches
call ..\bin\zassemble.bat cb_b_13121_patches
call ..\bin\zassemble.bat cb_b_13182_patches
call ..\bin\zassemble.bat cd_9452_patches
call ..\bin\zassemble.bat cd_12905_patches
call ..\bin\zassemble.bat cd_13182_patches

copy /b cb_b_9188_patches.bin+cd_9452_patches.bin .\out\patches_mfgtrinity_base.bin
copy /b cb_b_13121_patches.bin+cd_12905_patches.bin .\out\patches_mfgcorona_base.bin
copy /b cb_b_13182_patches.bin+cd_13182_patches.bin .\out\patches_mfg13182corona_base.bin

rm -f *.bin


endlocal
@pause
