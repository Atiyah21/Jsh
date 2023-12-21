#include "redirection.h"

int fd = -1;
/*@return 0 si pas de redirection
 * -1 si erreur
 * 1 si il y a une redirection
 */
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
                fprintf(stderr, "jsh: Missing arguement\n");
                return -1;
            }
            else
            {
                fd = open(ligne[i + 1], O_RDONLY);
                dup2(fd, STDIN_FILENO);
                ligne[i] = NULL;
                if (fd == -1)
                {
                    fprintf(stderr, "jsh: File doesn't exist\n");
                    return -1;
                }
                tmp = 1;
            }
        }
        else if (strcmp(ligne[i], ">") == 0)
        {
            if (i + 1 >= nbw)
            {
                fprintf(stderr, "jsh: Missing arguement\n");
                return -1;
            }
            else
            {
                fd = open(ligne[i + 1], O_WRONLY | O_CREAT | O_EXCL, 0666);
                dup2(fd, STDOUT_FILENO);
                ligne[i] = NULL;
                if (fd == -1)
                {
                    fprintf(stderr, "jsh: cannot overwrite existing file\n");
                    return -1;
                }
                tmp = 1;
            }
        }
        else if (strcmp(ligne[i], ">>") == 0)
        {
            if (i + 1 >= nbw)
            {
                fprintf(stderr, "jsh: Missing arguement\n");
                return -1;
            }
            else
            {
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
                fprintf(stderr, "jsh: Missing arguement\n");
                return -1;
            }
            else
            {
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
                fprintf(stderr, "jsh: Missing arguement\n");
                return -1;
            }
            else
            {
                fd = open(ligne[i + 1], O_WRONLY | O_CREAT | O_EXCL, 0666);
                dup2(fd, STDERR_FILENO);
                ligne[i] = NULL;
                if (fd == -1)
                {
                    fprintf(stderr, "jsh: cannot overwrite existing file\n");
                    return -1;
                }
                tmp = 1;
            }
        }
        else if (strcmp(ligne[i], "2>>") == 0)
        {
            if (i + 1 >= nbw)
            {
                fprintf(stderr, "jsh: Missing arguement\n");
                return -1;
            }
            else
            {
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
                fprintf(stderr, "jsh: Missing arguement\n");
                return -1;
            }
            else
            {
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
