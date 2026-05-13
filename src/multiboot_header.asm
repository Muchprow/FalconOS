section .multiboot
    align 4
    dd 0x1BADB002            ; Magic number
    dd 0x00                  ; Flags
    dd - (0x1BADB002 + 0x00) ; Checksum

section .text
global _start
extern main

_start:
    mov esp, stack_top
    
    call main
    
    cli
.hlt:
    hlt
    jmp .hlt

section .bss
align 16
stack_bottom:
    resb 16384 
stack_top:
