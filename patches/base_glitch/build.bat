@echo off
setlocal
set BASE_SRC=base_build
set BASE_NME=base_trin.bin

rm -f *.bin

call ..\bin\zassemble.bat cb_b_4577_patches
call ..\bin\zassemble.bat cb_b_5772_patches
call ..\bin\zassemble.bat cb_b_6752_patches
call ..\bin\zassemble.bat cb_b_9188_patches
call ..\bin\zassemble.bat cb_b_13121_patches
call ..\bin\zassemble.bat cb_b_fake
call ..\bin\zassemble.bat cd_8453_patches
call ..\bin\zassemble.bat cd_9452_patches
call ..\bin\zassemble.bat cd_12905_patches

copy /b cb_b_9188_patches.bin+cd_9452_patches.bin .\out\patches_trinity_base.bin
copy /b cb_b_13121_patches.bin+cd_12905_patches.bin .\out\patches_corona_base.bin
copy /b cb_b_fake.bin+cd_8453_patches.bin .\out\patches_fat_base.bin

copy /b cb_b_4577_patches.bin+cd_9452_patches.bin .\out\patches_g2zephyr_base.bin
copy /b cb_b_5772_patches.bin+cd_9452_patches.bin .\out\patches_g2falcon_base.bin
copy /b cb_b_6752_patches.bin+cd_9452_patches.bin .\out\patches_g2jasper_base.bin
copy /b cb_b_9188_patches.bin+cd_9452_patches.bin .\out\patches_g2trinity_base.bin
copy /b cb_b_13121_patches.bin+cd_12905_patches.bin .\out\patches_g2corona_base.bin

copy /b .\out\patches_fat_base.bin H:\x360sdk\__NAND\nandbuild_trin\patches\14719\base\
rm -f *.bin


endlocal
@pause
