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
    while ((fgets((line), sizline, desc))) {
        n = strlen(line);
        *(line + n) = '\0';
        /*
         * check to see if command line extends onto
         * next line. If so, append next line to command line
         */

        if (*(line + n - 2) == '\\' && *(line + n - 1) == '\n') {
            *(line + n) = ' ';
            *(line + n - 1) = ' ';
            *(line + n - 2) = ' ';
            continue;   /* read next line */
        }
        return n;      /* all done */
    }
    return 0;
}
