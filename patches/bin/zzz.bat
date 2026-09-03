
xenon-as.exe -be -many -mregnames test.s -o zzz.elf
xenon-objcopy.exe zzz.elf -O binary zzz.bin

xenon-objdump.exe -mpowerpc:common -EB -b binary -D zzz.bin
@pause


