rl78-elf-as.exe -g -o %~dp0\test.o %~dp0\test.asm
rl78-elf-ld.exe -g -T %~dp0\test_link.ld -o %~dp0\test.elf %~dp0\test.o
rl78-elf-objcopy.exe -O binary %~dp0\test.elf %~dp0\test.bin
del %~dp0\test.o