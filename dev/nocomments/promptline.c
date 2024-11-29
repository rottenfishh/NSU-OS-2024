#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "shell.h"

int promptline(char *prompt, char *line, int sizline)
{
    int n = 0;
    int bytes_read = 0;
    write(STDOUT_FILENO, prompt, strlen(prompt));
    FILE* desc;
    int len = 0;
    char* res = NULL;
    if (script) {
        desc = script;
    } else {
        desc = stdin;
    }
    while ((fgets((line + n), sizline - n, desc))) {
        len = strlen(line + n); 
        n += len - 1; 
        if (len > 0 && *(line + n) == '\n') { 
            *(line + n) = '\0';
        }
        if (len >= 2 && *(line + n - 1) == '\\') { 
            *(line + n) = ' '; 
            *(line + n - 1) = ' ';  
            continue;      
        }
        return n;      
    }
    return -1;
}
