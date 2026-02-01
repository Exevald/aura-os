[BITS 32]
[ORG 0x300000]

start:
    mov eax, 1
    mov ebx, msg_prompt
    int 0x80

    mov eax, 2
    mov ebx, input_buffer
    mov ecx, 32
    int 0x80

    mov eax, 1
    mov ebx, msg_after
    int 0x80

    mov eax, 1
    mov ebx, input_buffer
    int 0x80

    mov eax, 1
    mov ebx, newline
    int 0x80

    mov eax, 3
    mov ebx, 0
    int 0x80

    ret

msg_prompt db "What is your name? ", 0
msg_after  db "Nice to meet you, ", 0
newline    db 10, 0
input_buffer times 32 db 0