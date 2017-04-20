utils = util/pch_int_hmap.c util/vptr_int_hmap.c util/pch.c util/vptr.c util/int_stack.c util/int.c
sources = $(utils) lexer.c parser.c parse_tree.c ast.c symbol.c ast_gen.c error.c symbol_table.c compiler.c addr.c op.c type.c ircode.c x86.c compile_x86.c
output_file = compiler
debug_flags = -g -Wall -Wpedantic -o $(output_file)
release_flags = -O2 -o $(output_file)

sub:
	gcc $(debug_flags) $(sources) driver_sub.c
debug:
	gcc $(debug_flags) $(sources) driver.c
dbgmem:
	gcc $(debug_flags) -DLOG_MEM $(sources) driver.c
release:
	gcc $(release_flags) $(sources) driver.c
