[BITS 32]
section .text.entry
global _start
extern kernel_main

_start:
    call kernel_main
    cli
.halt:
    hlt
    jmp .halt