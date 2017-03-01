sources = lexer.c parser.c driver.c util/pch_int_hmap.c util/pch.c util/int_stack.c
debug_flags = -g -Wall -Wpedantic -Wno-sign-compare
release_flags = -O2

debug:
	gcc $(debug_flags) $(sources)
release:
	gcc $(release_flags) $(sources)
