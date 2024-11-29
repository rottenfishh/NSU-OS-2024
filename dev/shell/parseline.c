#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "shell.h"

static char *blankskip(register char *);
int is_space(char sym);

job* parseline(char *line){
    struct command cmds[MAXCMDS];
    int nargs, ncmds;
    register char *s;
    char aflg = 0;
    int rval;
    register int i;
    static char delim[] = " \t|&<>;\n";

    process* head_p = (process*) malloc(sizeof(process));
    if (head_p == NULL) {
        perror("malloc error");
        exit(1);
    }
    job* j = malloc(sizeof(job));
    if (j == NULL) {
        perror("malloc error");
        exit(1);
    }

    j->processes = head_p;
    j->job_pgid = 0;
    j->next = NULL;
    j->prev = NULL;
    j->appfile = j->outfile = j->infile = (char*) NULL;
    j->idx = 0;
    nargs = ncmds = rval = 0;
    int curr_idx = 0;
    int pipe_flag = 0;

    process* curr_p = head_p;
    job* curr_job = j;
    s = line;
    cmds[0].cmdargs[0] = (char *) NULL;
    for (i = 0; i < MAXCMDS; i++)
        cmds[i].cmdflag = 0;

    while (s && *s) {
        s = blankskip(s);
        if (!*s) break; 
        switch(*s) {
            /*case '\n':
                printf("sosz\n");
                if (pipe_flag == 0) {
                    curr_idx = ncmds;
                    curr_job->bg = 0;
                    printf("bbg 0\n");
                }
                pipe_flag = 0;
                *s++ = '\0';
                ++ncmds;
                nargs = 0;
                curr_job->next = (job*) malloc(sizeof(job));
                curr_job->next->prev = curr_job;
                if (rval != 0 && cmds->cmdargs) {
                    build_job(curr_job, cmds, curr_idx, rval);
                    printf("built %s bg %d\n", curr_job->job_name, curr_job->bg);
                }
                curr_job = curr_job->next;
                curr_job->next = NULL;
                printf("nargs %d ncmds %d curr_idx %d rval %d\n", nargs, ncmds, curr_idx, rval);
                printf("bg %d\n", curr_job->bg);
                curr_idx = ncmds;
                break;*/
            case '&': 
                if (nargs == 0) {
                    fprintf(stderr, "syntax error near unexcepted token &\n");
                    return NULL;
                }
                s++;
                if (*(++s) == '&') {
                    fprintf(stderr, "syntax error near unexcepted token &&\n");
                    return NULL;
                }
                
                s--;
                if (pipe_flag == 0)
                    curr_idx = ncmds;
                pipe_flag = 0;
                *s++ = '\0';
                ++ncmds;
                nargs = 0;
                curr_job->next = (job*) malloc(sizeof(job));
                if (curr_job->next == NULL) {
                    perror("malloc error");
                    exit(1);
                }
                curr_job->next->prev = curr_job;
                curr_job->bg = 1;
                printf("nargs %d ncmds %d curr_idx %d rval %d \n", nargs, ncmds, curr_idx, rval);
                printf("bg %d\n", curr_job->bg);
                if (rval != 0 && cmds->cmdargs) {
                    build_job(curr_job, cmds, curr_idx, rval);
                    printf("built %s bg %d\n", curr_job->job_name, curr_job->bg);
                }
                curr_job = curr_job->next;
                curr_job->next = NULL;
                curr_idx = ncmds;
                break;
            case ';':
                if (nargs == 0) {
                    fprintf(stderr, "syntax error near unexcepted token ;\n");
                    return NULL;
                }
                s++;
                if (*(++s) == ';') {
                    fprintf(stderr, "syntax error near unexcepted token ;;\n");
                    return NULL;
                }
                s--;
                if (pipe_flag == 0)
                    curr_idx = ncmds;
                pipe_flag = 0;
                *s++ = '\0';
                ++ncmds;
                nargs = 0;
                curr_job->next = (job*) malloc(sizeof(job));
                curr_job->next->prev = curr_job;
                printf("nargs %d ncmds %d curr_idx %d rval %d \n", nargs, ncmds, curr_idx, rval);
                printf("bg %d\n", curr_job->bg);
                if (rval != 0 && cmds->cmdargs) {
                    build_job(curr_job, cmds, curr_idx, rval);
                    printf("built %s bg %d\n", curr_job->job_name, curr_job->bg);
                }
                curr_job = curr_job->next;
                curr_job->next = NULL;
                curr_idx = ncmds;
                break;
            case '>':
                if (*(s+1) == '>') {
                    ++aflg;
                    *s++ = '\0';
                }
                *s++ = '\0';
                s = blankskip(s);
                if (!*s) {
                    fprintf(stderr, "syntax error\n");
                    return NULL;
                }

                if (aflg){
                    curr_job->appfile = s;
                } else {
                    curr_job->outfile = s;
                }
                s = strpbrk(s, delim);
                if (s) {
                    if (is_space(*s))
                        *s++ = '\0';
                }
                break;
            case '<':
                *s++ = '\0';
                s = blankskip(s);
                if (!*s) {
                    fprintf(stderr, "syntax error\n");
                    return NULL;
                }
                curr_job->infile = s;
                s = strpbrk(s, delim);
                if (is_space(*s))
                    *s++ = '\0';
                break;
            case '|':
                pipe_flag = 1;
                if (nargs == 0) {
                    fprintf(stderr, "syntax error\n");
                    return NULL;
                }
                cmds[ncmds++].cmdflag |= OUTPIP;
                cmds[ncmds].cmdflag |= INPIP;
                *s++ = '\0';
                nargs = 0;
                break;
            default:
                if (nargs == 0) 
                    rval = ncmds+1;
                cmds[ncmds].cmdargs[nargs++] = s;
                cmds[ncmds].cmdargs[nargs] = (char *) NULL;
                s = strpbrk(s, delim);
                if (s) {
                    printf("Delimiter found: '%c'\n", *s);
                    if (*s == '\n') {
                        printf("Handling newline.\n");
                    }
                }
                if (s) {
                    if (is_space(*s))
                        *s++ = '\0'; 
                }
                break;
        }
    } 
    if (pipe_flag == 0) {
        curr_idx = ncmds;
        curr_job->bg = 0;
        printf("lol bbg 0\n");
    }
    if (rval != 0 && cmds->cmdargs) {
        build_job(curr_job, cmds, curr_idx, rval); 
        printf("got built %s bg %d\n", curr_job->job_name, curr_job->bg);
    }
    else 
        return NULL;
    job* ptr;
    ptr = j;
    while (ptr) {
        printf("yes?\n");
        if (ptr->next && ptr->next->processes->argv.cmdargs[0] == NULL) {
            free_job(ptr->next);
            ptr->next = NULL;
        }
        ptr = ptr->next;
        printf("no\n");
    }
    printf("death\n");
    if (cmds[ncmds-1].cmdflag & OUTPIP) {
        if (nargs == 0) {
            fprintf(stderr, "syntax error\n");
            return NULL;
        }
    }

    return(j);
}

static char *
blankskip(register char *s){
    while (isspace(*s) && *s) ++s;
    return(s);
}

int is_space(char sym) {
    if (sym == ' ' || sym == '\t' || sym == '\r' || sym == '\n') 
        return 1;
    return 0;
}