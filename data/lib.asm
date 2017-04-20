section .bss
temp_integer:
    resb 8
section .data
print_integer_format:
    db "%hd", 10, 0
print_float_format:
    db "%f", 10, 0
read_integer_format:
    db "%hd", 0
read_float_format:
    db "%f", 0
read_string_format:
    db "%s", 0

section .text
print_integer:
    push rbx
    push rbx ; push twice to match 16-byte alignment requirement
    mov rdi, print_integer_format
    xor rsi, rsi
    mov si, cx
    xor rax, rax
    call printf
    pop rbx
    pop rbx
    ret
print_boolean:
    push rbx
    push rbx ; push twice to match 16-byte alignment requirement
    mov rdi, '0'
    and rcx, 0x0000000000000ff
    add rdi, rcx
    xor rax, rax
    call putchar
    mov rdi, 10
    xor rax, rax
    call putchar
    pop rbx
    pop rbx
    ret

read_integer:
    push rbx
    push rbx ; push twice to match 16-byte alignment requirement
    mov rdi, read_integer_format
    mov rsi, rcx
    xor rax, rax
    call scanf
    pop rbx
    pop rbx
    ret
read_boolean:
    push rbx
    push rcx
    mov rdi, read_integer_format
    mov rsi, temp_integer
    xor rax, rax
    call scanf
    pop rcx
    pop rbx
    mov ax, [temp_integer]
    cmp ax, 0
    setne byte [rcx]
    ret
