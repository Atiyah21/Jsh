#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include <linux/limits.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

extern void prompt(char *, int);
extern void running_status(int,int,char*,int);
extern void done_status(int,int,char*,int);
extern void killed_status(int,int,char*,int);
extern void stopped_status(int, int, char *,int);
int next(char** arg);
#endif
