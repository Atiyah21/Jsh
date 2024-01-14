#ifndef REDIRECTION_H
#define REDIRECTION_H

#include <linux/limits.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

extern int redirection(char **, int);
extern int fd;
extern int process_substitution(int, char **);
extern int command_pipe(char **, int, int);
#endif
