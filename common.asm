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
