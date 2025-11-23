[BITS 16]
[ORG 0x7C00]

start_jump:
    jmp short start
    nop

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov [boot_drive], dl
    mov ah, 0x0E
    mov al, 'S'
    int 0x10

    call load_kernel
    call switch_to_pm

    jmp $

load_kernel:
    mov ah, 0x0E
    mov al, 'L'
    int 0x10


    xor ax, ax
    mov es, ax
    mov bx, 0x8000

    mov ah, 0x02
    mov al, 15
    mov ch, 0
    mov dh, 0
    mov cl, 2
    mov dl, [boot_drive]
    int 0x13

    jc disk_error

    mov ah, 0x0E
    mov al, 'K'
    int 0x10
    ret

disk_error:
    mov ah, 0x0E
    mov al, 'E'
    int 0x10
    jmp $

switch_to_pm:
    mov ah, 0x0E
    mov al, 'P'
    int 0x10

    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp 0x08:init_pm

[BITS 32]
init_pm:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x90000

    jmp 0x8000

boot_drive db 0

gdt_start:
    dd 0x0, 0x0
gdt_code:
    dw 0xFFFF, 0x0000
    db 0x00, 10011010b, 11001111b, 0x00
gdt_data:
    dw 0xFFFF, 0x0000
    db 0x00, 10010010b, 11001111b, 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

times 510 - ($ - $$) db 0
dw 0xAA55