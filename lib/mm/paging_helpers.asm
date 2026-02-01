[BITS 32]
global loadPageDirectory
global enablePaging
global flushTLB

loadPageDirectory:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]
    mov cr3, eax
    mov esp, ebp
    pop ebp
    ret

enablePaging:
    push ebp
    mov ebp, esp
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    mov esp, ebp
    pop ebp
    ret

flushTLB:
    mov eax, cr3
    mov cr3, eax
    ret