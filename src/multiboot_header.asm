section .multiboot
    align 4
    dd 0x1BADB002            ; Magic number
    dd 0x00                  ; Flags
    dd - (0x1BADB002 + 0x00) ; Checksum

section .text
global _start
extern main

_start:
    ; Устанавливаем стек в безопасное место
    mov esp, stack_top
    
    ; Передаем управление в Си
    call main
    
    ; Если ядро решит выйти (чего не должно быть), зацикливаем
    cli
.hlt:
    hlt
    jmp .hlt

section .bss
align 16
stack_bottom:
    resb 16384 ; Выделяем 16 КБ под стек
stack_top: