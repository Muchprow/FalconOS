@echo off
set NASM=".\asm\nasm.exe"
set CLANG=clang
set LLD=ld.lld
set QEMU="D:\LIBS\msys2\mingw64\bin\qemu-system-x86_64.exe"

if exist build rd /s /q build
mkdir build

%NASM% -f elf32 src/multiboot_header.asm -o build/header.o
%CLANG% --target=i386-pc-none-elf -march=i386 -m32 -ffreestanding -fno-stack-protector -mno-sse -mno-mmx -O0 -c src/kernel.c -o build/kernel.o
%LLD% -m elf_i386 -T linker.ld build/header.o build/kernel.o -o build/falconos.elf

%QEMU% -kernel build/falconos.elf
pause