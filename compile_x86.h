#ifndef H_COMPILE_X86
#define H_COMPILE_X86

#include <stdio.h>
#include "symbol_table.h"
#include "ast.h"

void compile_program_to_x86(ProgramNode*, pSD, FILE*);

#endif  // H_COMPILE_X86
