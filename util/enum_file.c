#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "enum_file.h"

#define ENUM_BUFSIZE 100
static char enum_buffer[ENUM_BUFSIZE];

void read_enum_file(const char* fname, char list[][ENUM_ENTRY_SIZE])
{
    FILE* fp = fopen(fname, "r");
    if(fp == NULL)
    {
        perror("read_enum_file");
        fprintf(stderr, "Couldn't open file %s\n", fname);
        exit(1);
    }
    char* s;
    int i = -1, si, di;

    while((s = fgets(enum_buffer, ENUM_BUFSIZE, fp)) != NULL)
    {
        if(s == NULL)
            break;
        si = di = 0;
        bool copying = false;
        for(si=0; s[si] != '\0' && s[si] != '/' && s[si] != '\n'; ++si)
        {
            if(s[si] == ',' || s[si] == ' ' || s[si] == '\r' || s[si] == '\t')
            {
                if(copying)
                {
                    copying = false;
                    list[i][di] = '\0';
                }
            }
            else
            {
                if(!copying)
                {
                    copying = true;
                    i++;
                    di = 0;
                }
                list[i][di++] = s[si];
            }
        }
    }       
}
