#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "ast_gen.h"
#include "compiler.h"
#include "error.h"

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
            lexer_main(ifp, stdout, 0, print_token);
            break;
        case 2:
            parser_main(ifp, stdout, 0, print_tree);
            break;
        case 3:
            ast_gen_main(ifp, stdout, NULL, 0);
            break;
        case 4:
            ast_gen_main(ifp, NULL, stdout, 1);
            break;
        case 5:
            print_error_req = false;
            compiler_main(ifp, stdout, stdout, 0, 0);
            break;
        case 6:
            error_stream = stdout;
            compiler_main(ifp, stdout, NULL, 0, 0);
            break;
        case 7:
            compiler_main(ifp, ofp, NULL, 2, 0);
            break;
        default:
            fprintf(stderr, "Invalid choice!\n");
            break;
    }

    fprintf(stderr, "%d errors\n", error_count);
    if(warning_count > 0)
        fprintf(stderr, "%d warnings\n", warning_count);

    if(ifp != stdin)
        fclose(ifp);
    if(ofp != stdout && ofp != stderr)
        fclose(ofp);

    return 0;
}
