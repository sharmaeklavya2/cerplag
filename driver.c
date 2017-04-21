#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "compiler.h"
#include "ast_gen.h"

#define USAGE_SIZE 512

static char usage_template[] = "usage: %s (-l | -p | -a | -t | -s | -i | -c) (infile | --) [-v] [-o outfile] OR %s infile outfile\n";
static char usage[USAGE_SIZE];

int cry_error()
{
    fprintf(stderr, "%s", usage);
    return 2;
}

int main(int argc, char* argv[])
{
    snprintf(usage, USAGE_SIZE, usage_template, argv[0], argv[0]);

    // parse command-line arguments
    int i;
    char* ifname = NULL;
    char* ofname = "-1";
    char *arg1 = NULL, *arg2 = NULL;
    char type = '0';
    bool saw_flags = false;
    int verbosity = 0;
    for(i=1; i<argc; ++i)
    {
        char* argvi = argv[i];
        if(argvi[0] == '-')
        {
            saw_flags = true;
            char ch = argvi[1];
            switch(ch)
            {
            case 'h':
                printf("%s", usage);
                break;
            case 'v':
                verbosity++;
                break;
            case 'l':
            case 'p':
            case 'a':
            case 't':
            case 's':
            case 'i':
            case 'c':
                type = ch;
                if(i+1 < argc)
                    ifname = argv[++i];
                break;
            case 'o':
                if(i+1 < argc)
                    ofname = argv[++i];
                break;
            default:
                return cry_error();
            }
        }
        else if(!saw_flags)
        {
            if(arg1 == NULL)
                arg1 = argvi;
            else if(arg2 == NULL)
                arg2 = argvi;
        }
        else
            return cry_error();
    }
    if(saw_flags)
    {
        if(type == '0' || ifname == NULL)
            return cry_error();
    }
    else
    {
        type = 'c';
        if(arg1 == NULL)
            return cry_error();
        ifname = arg1;
        if(arg2 == NULL)
            ofname = "-1";
        else
            ofname = arg2;
    }

    // open files
    FILE* ifp = NULL;
    if(strcmp(ifname, "--") == 0)
        ifp = stdin;
    else
    {
        ifp = fopen(ifname, "r");
        if(ifp == NULL)
        {
            perror(NULL);
            fprintf(stderr, "Error while opening %s\n", ifname);
            return 1;
        }
    }
    FILE* ofp = NULL;
    if(strcmp(ofname, "-1") == 0)
        ofp = stdout;
    else if(strcmp(ofname, "-2") == 0)
        ofp = stderr;
    else
    {
        ofp = fopen(ofname, "w");
        if(ofp == NULL)
        {
            perror(NULL);
            fprintf(stderr, "Error while opening %s\n", ofname);
            return 1;
        }
    }

    // call appropriate function
    //int retval = 0;
    if(type == 'l')
        lexer_main(ifp, ofp, verbosity, print_token);
    else if(type == 'p')
        parser_main(ifp, ofp, verbosity, print_tree);
    else if(type == 'a')
        ast_gen_main(ifp, ofp, stderr, verbosity);
    else if(type == 't')
        compiler_main(ifp, ofp, NULL, 0, verbosity);
    else if(type == 's')
        compiler_main(ifp, ofp, stderr, 0, verbosity);
    else if(type == 'i')
        compiler_main(ifp, ofp, NULL, 1, verbosity);
    else if(type == 'c')
        compiler_main(ifp, ofp, NULL, 2, verbosity);

    if(ifp != stdin)
        fclose(ifp);
    if(ofp != stdout && ofp != stderr)
        fclose(ofp);

    if(error_count > 0)
        fprintf(stderr, "%d errors\n", error_count);
    if(warning_count > 0)
        fprintf(stderr, "%d warnings\n", warning_count);
    return error_count;
}
