sources = lexer.c parser.c util/pch_int_hmap.c util/vptr_int_hmap.c util/pch.c util/vptr.c util/int_stack.c parse_tree.c
debug_flags = -g -Wall -Wpedantic -Wno-sign-compare -Wno-pointer-to-int-cast
release_flags = -O2

sub:
	gcc $(debug_flags) $(sources) driver_sub.c -o stage1exe
debug:
	gcc $(debug_flags) $(sources) driver.c
release:
	gcc $(release_flags) $(sources) driver.c
