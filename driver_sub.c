#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "ast_gen.h"
#include "compiler.h"

#define USAGE_SIZE 512

static char usage_template[] = "usage: %s input_file [output_file]\n";
static char usage[USAGE_SIZE];

int cry_error()
{
    fprintf(stderr, "%s", usage);
    return 2;
}

int main(int argc, char* argv[])
{
    snprintf(usage, USAGE_SIZE, usage_template, argv[0]);
    if(argc < 2 || argc > 3)
        return cry_error();

    // open files
    FILE* ifp = fopen(argv[1], "r");
    if(ifp == NULL)
    {
        fprintf(stderr, "Couldn't open file %s:\n", argv[1]);
        perror(NULL);
        return 1;
    }
    FILE* ofp = stdout;
    if(argc == 3)
    {
        ofp = fopen(argv[2], "w");
        if(ofp == NULL)
        {
            fprintf(stderr, "Couldn't open file %s:\n", argv[2]);
            perror(NULL);
            return 1;
        }
    }

    fprintf(stderr, "Level 4: Symbol table, Type checking, Semantic rules, Assembly code generation.\n");

    fprintf(stderr, "Enter 1 to print token list.\n");
    fprintf(stderr, "Enter 2 to print parse tree in pre-order traversal.\n");
    fprintf(stderr, "Enter 3 to print AST in pre-order traversal.\n");
    fprintf(stderr, "Enter 4 to display compression percent.\n");
    fprintf(stderr, "Enter 5 to display symbol table.\n");
    fprintf(stderr, "Enter 6 to check code for semantic errors.\n");
    fprintf(stderr, "Enter 7 to produce assembly code.\n");
    int choice;
    scanf("%d",&choice);

    switch(choice){
        case 1:
            return lexer_main(ifp, stdout, 0, print_token_sub);
        case 2:
            return parser_main(ifp, stdout, 0, print_tree);
        case 3:
            return ast_gen_main(ifp, stdout, 0);
        case 4:
            return ast_gen_main(ifp, stdout, 1);
        case 5:
            return compiler_main(ifp, stdout, 1, 0);
        case 6:
            return compiler_main(ifp, stdout, 1, 0);
        case 7:
            return compiler_main(ifp, stdout, 3, 0);
        default:
            fprintf(stderr, "Invalid choice!\n");
            break;
    }
}
