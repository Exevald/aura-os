[BITS 16]
[ORG 0x7C00]

CODE_SEL equ 0x08
DATA_SEL equ 0x10
KERNEL_ADDR equ 0x00008000
boot_drive db 0

start:
    mov al, 'A'
    mov ah, 0x0E
    int 0x10

    cli
    mov [boot_drive], dl
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov ax, 0x0003
    int 0x10

    call reset_and_load
    mov si, err_msg
    call print_string
    jmp halt

halt:
    cli
    hlt
    jmp halt

reset_and_load:
    mov al, 'R'
    mov ah, 0x0E
    mov bh, 0
    int 0x10

    xor ah, ah
    mov dl, [boot_drive]
    int 0x13
    jc disk_reset_error

    mov al, 'L'
    mov ah, 0x0E
    mov bh, 0
    int 0x10

    mov ah, 0x02
    mov al, 1
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [boot_drive]
    mov ax, 0x0800
    mov es, ax
    mov bx, 0
    int 0x13
    jc load_error

    mov al, 'J'
    mov ah, 0x0E
    mov bh, 0
    int 0x10

    jmp enable_protected_mode

disk_reset_error:
    mov al, 'S'
    mov ah, 0x0E
    mov bh, 0
    int 0x10
    ret

load_error:
    mov [error_code], ah

    mov al, 'E'
    mov ah, 0x0E
    mov bh, 0
    int 0x10

    mov al, [error_code]
    shr al, 4
    call print_hex_nibble

    mov al, [error_code]
    and al, 0x0F
    call print_hex_nibble

    ret

print_hex_nibble:
    cmp al, 10
    jl .digit
    add al, 7
.digit:
    add al, '0'
    mov ah, 0x0E
    mov bh, 0
    int 0x10
    ret

error_code db 0

enable_protected_mode:
    lgdt [gdt_descriptor]

    mov eax, cr0
    or al, 0x01
    mov cr0, eax

    jmp CODE_SEL:protected_mode_start


print_string:
    mov ah, 0x0E
    mov bh, 0

.repeat:
    lodsb
    or al, al
    jz .done
    int 0x10
    jmp .repeat

.done:
    ret

align 2
msg: db 'Bootloader Loaded.', 13, 10, 0
err_msg: db 'Kernel load FAILED! System halted.', 13, 10, 0

gdt_start:
    dd 0x0, 0x0

    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00

    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd 0x7C00 + (gdt_start - $$)

[BITS 32]
protected_mode_start:
    mov ax, DATA_SEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x90000

    jmp KERNEL_ADDR

times 510 - ($ - $$) db 0
dw 0xAA55