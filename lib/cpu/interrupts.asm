extern isr_handler

%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push byte 0
    push byte %1
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push byte %1
    jmp isr_common_stub
%endmacro

%macro IRQ 2
global irq%1
irq%1:
    cli
    push byte 0
    push byte %2
    jmp isr_common_stub
%endmacro

// отписать комментарии
isr_common_stub:
    pusha
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push esp
    call isr_handler
    add esp, 4
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa
    add esp, 8
    iret

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_ERRCODE   8
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 128

IRQ 0, 32
IRQ 1, 33