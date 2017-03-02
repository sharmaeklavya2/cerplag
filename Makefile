sources = lexer.c parser.c util/pch_int_hmap.c util/pch.c util/int_stack.c util/tree.c
debug_flags = -g -Wall -Wpedantic -Wno-sign-compare
release_flags = -O2

sub:
	gcc $(debug_flags) $(sources) driver_sub.c
debug:
	gcc $(debug_flags) $(sources) driver.c
release:
	gcc $(release_flags) $(sources) driver.c
