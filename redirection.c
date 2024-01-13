#include "redirection.h"
#include <linux/limits.h>


int fd = -1;
/*@return 0 si pas de redirection
 * -1 si erreur
 * 1 si il y a une redirection
 */

int next(char **arg)
{
    int i = 0;
    while (arg[i] != NULL)
    {
        if (strcmp(arg[i], ")") == 0)
            return i;
        i++;
    }
    return -1;
}

int special(char **arg)
{
    int i = 0;
    while (arg[i] != NULL)
    {
        if (strcmp(arg[i], "<") == 0 || strcmp(arg[i], ">") == 0 || strcmp(arg[i], ">>") == 0 || strcmp(arg[i], ">|") == 0 || strcmp(arg[i], "2>") == 0 || strcmp(arg[i], "2>>") == 0 || strcmp(arg[i], "2>|") == 0 || strcmp(arg[i], "|") == 0 || strcmp(arg[i], "<(") == 0)
            return 1;
        i++;
    }
    return 0;
}

int redirection(char **ligne, int nbw)
{
    if (nbw == 1)
        return 0;
    int tmp = 0;
    for (int i = 0; ligne[i] != NULL; i++)
    {
        if (strcmp(ligne[i], "<") == 0)
        {
            if (i + 1 >= nbw)
            {
                write(2, "jsh: Missing arguement\n",24);
                return -1;
            }
            else
            {
                if(special(ligne+i+1)==1){
                    write(2, "jsh: Syntax error\n",24);
                    return -1;
                }
                fd = open(ligne[i + 1], O_RDONLY);
                dup2(fd, STDIN_FILENO);
                ligne[i] = NULL;
                if (fd == -1)
                {
                    write(2, "jsh: File doesn't exist\n",24);
                    return -1;
                }
                tmp = 1;
            }
        }
        else if (strcmp(ligne[i], ">") == 0)
        {
            if (i + 1 >= nbw)
            {
                write(2, "jsh: Missing arguement\n",24);
                return -1;
            }
            else
            {
                if(special(ligne+i+1)==1){
                    write(2, "jsh: Syntax error\n",24);
                    return -1;
                }
                fd = open(ligne[i + 1], O_WRONLY | O_CREAT | O_EXCL, 0666);
                dup2(fd, STDOUT_FILENO);
                ligne[i] = NULL;
                if (fd == -1)
                {
                    write(2, "jsh: cannot overwrite existing file\n",37);
                    return -1;
                }
                tmp = 1;
            }
        }
        else if (strcmp(ligne[i], ">>") == 0)
        {
            if (i + 1 >= nbw)
            {
                write(2, "jsh: Missing arguement\n",24);
                return -1;
            }
            else
            {
                if(special(ligne+i+1)==1){
                    write(2, "jsh: Syntax error\n",24);
                    return -1;
                }
                fd = open(ligne[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0666);
                dup2(fd, STDOUT_FILENO);
                ligne[i] = NULL;
                if (fd == -1)
                    return -1;
                tmp = 1;
            }
        }
        else if (strcmp(ligne[i], ">|") == 0)
        {
            if (i + 1 >= nbw)
            {
                write(2, "jsh: Missing arguement\n",24);
                return -1;
            }
            else
            {
                if(special(ligne+i+1)==1){
                    write(2, "jsh: Syntax error\n",24);
                    return -1;
                }
                fd = open(ligne[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
                dup2(fd, STDOUT_FILENO);
                ligne[i] = NULL;
                if (fd == -1)
                {
                    return -1;
                }
                tmp = 1;
            }
        }
        else if (strcmp(ligne[i], "2>") == 0)
        {
            if (i + 1 >= nbw)
            {
                write(2, "jsh: Missing arguement\n",24);
                return -1;
            }
            else
            {
                if(special(ligne+i+1)==1){
                    write(2, "jsh: Syntax error\n",24);
                    return -1;
                }
                fd = open(ligne[i + 1], O_WRONLY | O_CREAT | O_EXCL, 0666);
                dup2(fd, STDERR_FILENO);
                ligne[i] = NULL;
                if (fd == -1)
                {
                    write(2, "jsh: cannot overwrite existing file\n",37);
                    return -1;
                }
                tmp = 1;
            }
        }
        else if (strcmp(ligne[i], "2>>") == 0)
        {
            if (i + 1 >= nbw)
            {
                write(1, "jsh: Missing arguement\n",24);
                return -1;
            }
            else
            {
                if(special(ligne+i+1)==1){
                    write(2, "jsh: Syntax error\n",24);
                    return -1;
                }
                fd = open(ligne[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0666);
                dup2(fd, STDERR_FILENO);
                ligne[i] = NULL;
                if (fd == -1)
                {
                    return -1;
                }
                tmp = 1;
            }
        }
        else if (strcmp(ligne[i], "2>|") == 0)
        {
            if (i + 1 >= nbw)
            {
                write(2, "jsh: Missing arguement\n",24);
                return -1;
            }
            else
            {
                if(special(ligne+i+1)==1){
                    write(2, "jsh: Syntax error\n",24);
                    return -1;
                }
                fd = open(ligne[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
                dup2(fd, STDERR_FILENO);
                ligne[i] = NULL;
                if (fd == -1)
                {
                    return -1;
                }
                tmp = 1;
            }
        }
    }
    return tmp;
}


