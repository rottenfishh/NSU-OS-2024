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
        len = strlen(line + n); // length of string
        n += len - 1; // now n points to last symbol of readed string
        if (len > 0 && *(line + n) == '\n') { // change last symbol to \0 if it was \n
            *(line + n) = '\0';
        }
        if (len >= 2 && *(line + n - 1) == '\\') { // if we have continuation(last symbol \ before \0)
            *(line + n) = ' '; //change \0 to backspace
            *(line + n - 1) = ' ';   //change \ to backspace
            continue;          //continue reading string (now to buffer starting from line up to the last readed symbol pointed by n)
        }
        return n;      
    }
    return -1;
}
