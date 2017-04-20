section .bss
temp_integer:
    resb 8
section .data
integer_format:
    db "%hd", 0
float_format:
    db "%f", 0
string_format:
    db "%s", 0
string_false:
    db "0", 0
string_true:
    db "1", 0
integer_format2:
    db "%hd", 10, 0
float_format2:
    db "%f", 10, 0
string_format2:
    db "%s", 10, 0
string_false2:
    db "0", 10, 0
string_true2:
    db "1", 10, 0

section .text
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
