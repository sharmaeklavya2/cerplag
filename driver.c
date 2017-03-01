#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"

#define USAGE_SIZE 512

static char usage_template[] = "usage: %s (-l | -p) (infile | --) [-v] [-o outfile]\n";
static char usage[USAGE_SIZE];

int cry_error()
{
    fprintf(stderr, "%s", usage);
    return 2;
}

int main(int argc, char* argv[])
{
    snprintf(usage, USAGE_SIZE, usage_template, argv[0]);

    // parse command-line arguments
    int i;
    char* ifname = NULL;
    char* ofname = "-1";
    char type = '0';
    int verbosity = 0;
    for(i=1; i<argc; ++i)
    {
        char* argvi = argv[i];
        if(argvi[0] == '-')
        {
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
        else
            return cry_error();
    }
    if(type == '0' || ifname == NULL)
        return cry_error();

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
    int retval = 0;
    if(type == 'l')
        retval = lexer_main(ifp, ofp, verbosity);
    else
        retval = parser_main(ifp, ofp, verbosity);

    if(ifp != stdin)
        fclose(ifp);
    if(ofp != stdout && ofp != stderr)
        fclose(ofp);
    return retval;
}
