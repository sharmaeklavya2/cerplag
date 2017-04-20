section .text
extern scanf
read_boolean_array:
    ; base addr in rcx, size in rdx
    push rbx
    push rbx ; push twice to match 16-byte alignment requirement
read_boolean_beg:
    mov rdi, integer_format
    mov rsi, temp_integer
    xor rax, rax
    push rcx
    push rdx
    call scanf
    pop rdx
    pop rcx
    mov ax, [temp_integer]
    cmp ax, 0
    setne byte [rcx]
    inc rcx
    dec rdx
    cmp rdx, 0
    jnz read_boolean_beg
    pop rbx
    pop rbx
    ret
