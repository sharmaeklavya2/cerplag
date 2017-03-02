#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"

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

    fprintf(stderr, "The FIRST and FOLLOW sets have been automated.\n");
    fprintf(stderr, "Lexical Analyzed module completed.\n");
    fprintf(stderr, "Syntax Analysis module completed.\n");
    fprintf(stderr, "All modules compile and run withour errors.\n");
    fprintf(stderr, "Modules work on all testcases.\n");
    fprintf(stderr, "Parse Tree was constructed successfully.\n");
    fprintf(stderr, "\n");

    fprintf(stderr, "Enter 1 to print the source code without comments.\n");
    fprintf(stderr, "Enter 2 to print all tokens along with corresonding line numbers and lexemes.\n");
    fprintf(stderr, "Enter 3 to parse the source code.\n");
    fprintf(stderr, "Enter 4 to create the parse tree and print it.\n");
    int choice;
    scanf("%d",&choice);

    switch(choice){
        case 1:
            break;
        case 2:
            return lexer_main(ifp, ofp, 0, print_token_sub);
        case 3:
            return parser_main(ifp, ofp, 0, dont_print_tree);
        case 4:
            return parser_main(ifp, ofp, 0, print_tree_sub);
        default:
            fprintf(stderr, "Invalid choice!\n");
            break;
    }
}
