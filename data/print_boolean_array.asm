section .text
extern printf
extern putchar
print_boolean_array:
    ; base addr in rcx, size in rdx
    push rbx
    push rbx ; push twice to match 16-byte alignment requirement
print_boolean_beg:
    cmp [rcx], byte 0
    jnz select_true_array
    mov rdi, string_false
    jmp selected_array
select_true_array:
    mov rdi, string_true
selected_array:
    xor rax, rax
    push rcx
    push rdx
    call printf
    pop rdx
    pop rcx
    inc rcx
    dec rdx
    cmp rdx, 0
    jz print_boolean_end
    mov rdi, ' '
    xor rax, rax
    push rcx
    push rdx
    call putchar
    pop rdx
    pop rcx
    jmp print_boolean_beg
print_boolean_end:
    mov rdi, 10
    xor rax, rax
    call putchar
    pop rbx
    pop rbx
    ret
