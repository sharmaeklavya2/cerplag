section .text
extern printf
print_integer:
    ; value in cx
    push rbx
    push rbx ; push twice to match 16-byte alignment requirement
    mov rdi, integer_format2
    xor rsi, rsi
    mov si, cx
    xor rax, rax
    call printf
    pop rbx
    pop rbx
    ret
