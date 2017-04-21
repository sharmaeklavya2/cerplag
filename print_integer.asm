section .text
extern printf
print_integer:
    ; value in cx
    push rbx
    push rbx ; push twice to match 16-byte alignment requirement
    mov rdi, integer_format2
    mov si, cx
    xor rax, rax
    call printf
    pop rbx
    pop rbx
    xor rsi, rsi
    xor rdi, rdi
    ret
