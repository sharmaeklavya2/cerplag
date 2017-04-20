section .text
extern printf
print_boolean:
    ; value in cl
    push rbx
    push rbx ; push twice to match 16-byte alignment requirement
    cmp cl, 0
    jnz select_true
    mov rdi, string_false2
    jmp selected
select_true:
    mov rdi, string_true2
selected:
    xor rax, rax
    call printf
    pop rbx
    pop rbx
    ret
