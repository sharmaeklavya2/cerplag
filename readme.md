# CErplag

CErplag is a compiler written in C which compiles Erplag source code to NASM code.

Erplag is a toy programming language designed for the 'Compiler Construction'
course at BITS Pilani. You can find the original language spec here:
https://github.com/sharmaeklavya2/erplag/wiki/original-spec.pdf

Currently CErplag doesn't generate code for function calls.

### Compiling CErplag

Simply execute

    $ make release

from the project root to create the compiler executable named `compiler`.

### Compiling erplag source files

Execute

    $ ./compiler <input_file> <output_file>

and the NASM assemby code will be written to `output_file`.

You can then compile `output_file` to `exec_file` using `nasm` and `gcc`:

    $ nasm -felf64 <output_file> -o out.o
    $ gcc out.o -o <exec_file>

You can also use `erplag_run.sh` to directly compile and run erplag source code:

    $ ./erplag_run.sh <input_file>

### Basic Code Layout

* `driver.c`: This is the entry point of the program.
* `lexer.c`: Tokenization and lexical analysis.
* `parser.c`:
    * Reads the output of lexer.
    * Calculates first and follow sets and creates parse table.
    * Constructs parse tree.
    * Reports parsing errors.
* `ast_gen.c`: Constructs AST using parse tree.
* `compiler.c`: Type checking and semantic analysis.
* `codegen.c`: Generates intermediate code.
* `compile_x86.c`: Convert intermediate representation to NASM assembly.

### Data Structures Used

* `token.h`: Token structure used by lexer and parser.
* `symbol.h`: Grammar symbol structure used as parse tree node.
* `ast.h`: Defines node structure of AST.
* `symbol_table.h`: Symbol table structure.
* `ircode.h`: Intermediate 3-address code representation.
* `addr.h`: Operand structure used in 3-address code.
* `x86.h`: NASM instruction representation.

### Automated Tests

    $ ./tester.sh tests/c*.erp

### Conventions

* A type of an erplag entity is represented as a pair of two integers, (`type`, `size`).
  If the object is an array, `size` holds the number of elements in the array
  and `type` is the type of each element. If the object is not an array,
  `size` is 0.
* A tree of symbol tables is referred to as a Symbol Database in cerplag's code.
