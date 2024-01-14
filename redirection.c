#include "redirection.h"

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

int mauvaise_suite(char **ligne, int index)
{
    return ((strcmp(ligne[index], "|") == 0) | (strcmp(ligne[index], "<") == 0) | (strcmp(ligne[index], ">") == 0) | (strcmp(ligne[index], ">>") == 0) |
            (strcmp(ligne[index], "<<") == 0) | (strcmp(ligne[index], "2>") == 0) |( strcmp(ligne[index], "<2") == 0) | (strcmp(ligne[index], "<|") == 0) | (strcmp(ligne[index], "|>") == 0));
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
                write(2, "jsh: Missing arguement\n", 24);
                return -1;
            }
            if (mauvaise_suite(ligne, i + 1))
            {
                write(2, "unexpected token\n", 18);
                return -1;
            }
            else
            {
                fd = open(ligne[i + 1], O_RDONLY);
                dup2(fd, STDIN_FILENO);
                ligne[i] = NULL;
                if (fd == -1)
                {
                    write(2, "jsh: File doesn't exist\n", 24);
                    return -1;
                }
                tmp = 1;
            }
        }
        else if (strcmp(ligne[i], ">") == 0)
        {
            if (i + 1 >= nbw)
            {
                write(2, "jsh: Missing arguement\n", 24);
                return -1;
            }
            if (mauvaise_suite(ligne, i + 1))
            {
                write(2, "unexpected token\n", 18);
                return -1;
            }
            else
            {
                fd = open(ligne[i + 1], O_WRONLY | O_CREAT | O_EXCL, 0666);
                dup2(fd, STDOUT_FILENO);
                ligne[i] = NULL;
                if (fd == -1)
                {
                    write(2, "jsh: cannot overwrite existing file\n", 37);
                    return -1;
                }
                tmp = 1;
            }
        }
        else if (strcmp(ligne[i], ">>") == 0)
        {
            if (i + 1 >= nbw)
            {
                write(2, "jsh: Missing arguement\n", 24);
                return -1;
            }
            if (mauvaise_suite(ligne, i + 1))
            {
                write(2, "unexpected token\n", 18);
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
                write(2, "jsh: Missing arguement\n", 24);
                return -1;
            }
            if (mauvaise_suite(ligne, i + 1))
            {
                write(2, "unexpected token\n", 18);
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
                write(2, "jsh: Missing arguement\n", 24);
                return -1;
            }
            if (mauvaise_suite(ligne, i + 1))
            {
                write(2, "unexpected token\n", 18);
                return -1;
            }
            else
            {
                fd = open(ligne[i + 1], O_WRONLY | O_CREAT | O_EXCL, 0666);
                dup2(fd, 2);
                ligne[i] = NULL;
                if (fd == -1)
                {
                    write(2, "jsh: cannot overwrite existing file\n", 37);
                    return -1;
                }
                tmp = 1;
            }
        }
        else if (strcmp(ligne[i], "2>>") == 0)
        {
            if (i + 1 >= nbw)
            {
                write(2, "jsh: Missing arguement\n", 24);
                return -2;
            }
            else if (mauvaise_suite(ligne, i + 1))
            {
                write(2, "unexpected token\n", 18);
                return -2;
            }
            else
            {
                fd = open(ligne[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0666);
                dup2(fd, 2);
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
                write(2, "jsh: Missing arguement\n", 24);
                return -2;
            }
            else if (mauvaise_suite(ligne, i + 1))
            {
                write(2, "unexpected token\n", 18);
                return -2;
            }
            else
            {
                fd = open(ligne[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
                dup2(fd, 2);
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

int command_pipe(char **ligne, int nbw, int ret)
{
    int tmp = 0;
    for (int i = 0; ligne[i] != NULL; i++)
    {
        if (strcmp(ligne[i], "|") == 0)
        {
            if (i + 1 >= nbw)
            {
                write(2, "jsh: Missing arguement\n", 24); // TODO changer en write sur 2
                return -2;
            }
            else if (mauvaise_suite(ligne, i + 1))
            {
                write(2, "unexpected token\n", 18);
                return -2;
            }
            else
            {
                int stin = dup(0);
                int stout = dup(1);
                int fd[2];
                pipe(fd);
                int pid = fork();
                if (pid == 0)
                {
                    close(fd[0]);
                    dup2(fd[1], STDOUT_FILENO);
                    ligne[i] = NULL;
                    execute(i, ligne);
                    close(fd[1]);
                    exit(0);
                }
                else
                {
                    close(fd[1]);
                    dup2(fd[0], STDIN_FILENO);
                    // ligne = ligne + i + 1;
                    waitpid(pid, &ret, -1);
                    tmp = execute(nbw - i - 1, ligne + i + 1);
                    close(fd[0]);
                    dup2(stin, 0);
                    dup2(stout, 1);
                    return tmp;
                }
            }
        }
    }
    return -1;
}

int process_substitution(int nbw, char **ligne)
{
    int tmp = 0;
    for (int i = 0; ligne[i] != NULL; i++)
    {
        if (strcmp(ligne[i], "<(") == 0)
        {
            int j = next(ligne + i);
            if (j == -1)
            {
                write(2, "jsh: Missing arguements\n", 24);
                return -1;
            }
            else
            {
                // if(i==-1){
                //     write(2, "jsh: Panthesis not closed\n");
                //     return -1;
                // }
                // int e=0;
                char c[20];
                sprintf(c, "/tmp/tube%d", getpid());
                mkfifo(c, 0666); // On cree un tube avec mkfifo | O_NONBLOCK
                int file = open(c, O_RDWR | O_NONBLOCK);
                if (file == -1)
                {
                    write(2, "jsh: Process substitution error\n", 32); // TODO changer en write sur 2
                    return -1;
                }
                int stin = dup(STDIN_FILENO);
                int stout = dup(STDOUT_FILENO);
                dup2(file, STDOUT_FILENO);
                ligne[i] = NULL;
                ligne[i + j] = NULL;
                // int pid=fork();
                // if(pid==0){
                // printf("%s\n", ligne[i + 1]);
                execute(j - i, ligne + i + 1); // TODO segmentation ici
                close(file);
                dup2(stin, STDIN_FILENO);
                dup2(stout, STDOUT_FILENO);
                close(stin);
                close(stout);
                //   exit(0);
                // }else{
                // waitpid(pid, &e, -1);
                for (int y = i; y < j + i + 1; y++)
                    ligne[y] = NULL;
                char *nvligne[128];
                int cpt = 0;
                for (int s = 0; s < nbw; s++)
                { // on parcourt ligne,
                    if (s == i)
                    {
                        // char str1[128];
                        // strcpy(str1, c); // nom du fichier créé
                        nvligne[cpt] = c;
                        cpt++;
                    }
                    else if (ligne[s] != NULL)
                    {
                        // char str2[128];
                        // strcpy(str2, ligne[s]);
                        nvligne[cpt] = ligne[s];
                        cpt++;
                    }
                }
                nvligne[cpt] = NULL;
                tmp = execute(cpt, nvligne);
                unlink(c);
                remove(c);
                return tmp;
                // }
            }
        }
    }
    return -1;
}
