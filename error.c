#include <stdio.h>

int error_count;

void print_error(const char* category, int err_num, int line, int col,
    const char* lexeme, const char* err_code, const char* err_msg)
{
    error_count++;

    if(category != NULL)
        fprintf(stderr, "%s_", category);
    fprintf(stderr, "error");
    if(err_num >= 0)
        fprintf(stderr, "_%02d", err_num);
    fprintf(stderr, ": ");

    if(line > 0)
    {
        fprintf(stderr, "line %2d", line);
        if(col > 0)
            fprintf(stderr, ", col %2d", col);
        fprintf(stderr, ": ");
    }

    if(lexeme != NULL)
        fprintf(stderr, "\'%s\'", lexeme);

    if(err_code != NULL || err_msg != NULL)
        fprintf(stderr, "\n");
    if(err_code != NULL)
        fprintf(stderr, "%s: ", err_code);
    if(err_msg != NULL)
        fprintf(stderr, "%s", err_msg);

    fprintf(stderr, "\n\n");
}
