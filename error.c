#include <stdio.h>
#include <stdbool.h>
#include "error.h"

FILE* error_stream = NULL;
int error_count = 0;
int warning_count = 0;
bool print_error_req = true;

void print_error(const char* category, int type, int err_num, int line, int col,
    const char* lexeme, const char* err_code, const char* err_msg)
{
    if(!print_error_req)
        return;
    if(error_stream == NULL)
        error_stream = stderr;

    const char* type_str;
    if(type == ERROR) {
        type_str = "error";
        error_count++;
    }
    else {
        type_str = "warning";
        warning_count++;
    }

    if(category != NULL)
        fprintf(error_stream, "%s_", category);
    fprintf(error_stream, "%s", type_str);
    if(err_num >= 0)
        fprintf(error_stream, "_%02d", err_num);
    fprintf(error_stream, ": ");

    if(line > 0)
    {
        fprintf(error_stream, "line %2d", line);
        if(col > 0)
            fprintf(error_stream, ", col %2d", col);
        fprintf(error_stream, ": ");
    }

    if(lexeme != NULL)
        fprintf(error_stream, "\'%s\'", lexeme);

    if(err_code != NULL || err_msg != NULL)
        fprintf(error_stream, "\n");
    if(err_code != NULL)
        fprintf(error_stream, "%s: ", err_code);
    if(err_msg != NULL)
        fprintf(error_stream, "%s", err_msg);

    fprintf(error_stream, "\n\n");
}
