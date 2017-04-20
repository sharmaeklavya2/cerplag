section .text
extern printf
extern putchar
print_integer_array:
    ; base addr in rcx, size in rdx
    push rbx
    push rbx ; push twice to match 16-byte alignment requirement
print_integer_beg:
    mov rdi, integer_format
    xor rsi, rsi
    mov si, [rcx]
    xor rax, rax
    push rcx
    push rdx
    call printf
    pop rdx
    pop rcx
    add rcx, 2
    dec rdx
    cmp rdx, 0
    jz print_integer_end
    mov rdi, ' '
    xor rax, rax
    push rcx
    push rdx
    call putchar
    pop rdx
    pop rcx
    jmp print_integer_beg
print_integer_end:
    mov rdi, 10
    xor rax, rax
    call putchar
    pop rbx
    pop rbx
    ret
