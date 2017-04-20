section .data
print_integer_format:
    db "%hd", 10, 0
print_float_format:
    db "%f", 10, 0
read_integer_format:
    db "%hd", 0
read_float_format:
    db "%f", 0

section .text
print_integer:
    mov rdi, print_integer_format
    xor rsi, rsi
    mov si, cx
    xor rax, rax
    call printf
    ret
print_boolean:
    mov rdi, '0'
    add dl, cl
    xor rax, rax
    call putchar
    ret
print_float:
    ret

read_integer:
    mov rdi, read_integer_format
    mov rsi, rcx
    xor rax, rax
    call scanf
    ret
read_boolean:
    push rcx
    call getchar
    pop rcx
    sub rax, '0'
    mov [rcx], rax
    ret
read_float:
    mov rdi, read_float_format
    mov rsi, rcx
    xor rax, rax
    call scanf
    ret
