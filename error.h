extern int error_count;

void print_error(const char* category, int err_num, int line, int col,
    const char* lexeme, const char* err_code, const char* err_msg);
