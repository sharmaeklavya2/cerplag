section .text
extern scanf
read_integer_array:
    ; base addr in rcx, size in rdx
    push rbx
    push rbx ; push twice to match 16-byte alignment requirement
read_integer_beg:
    mov rdi, integer_format
    mov rsi, rcx
    xor rax, rax
    push rcx
    push rdx
    call scanf
    pop rdx
    pop rcx
    add rcx, 2
    dec rdx
    cmp rdx, 0
    jnz read_integer_beg
    pop rbx
    pop rbx
    ret
