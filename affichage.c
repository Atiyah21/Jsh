#include "affichage.h"

void prompt(char *pro, int num_jobs)
{
        char *actuel = malloc(PATH_MAX);
        getcwd(actuel, PATH_MAX);
        size_t taille_chemin = strlen(actuel);
        // s[0] = '\0';
        if (taille_chemin > 25)
        {

                sprintf(pro, "\001\033[91m\002[%d]\001\033[36m\002...%s\001\033[00m\002$ ", num_jobs, (actuel + taille_chemin - 22));
        }
        else
        {
                sprintf(pro, "\001\033[91m\002[%d]\001\033[36m\002%s\001\033[00m\002$ ", num_jobs, actuel);
        }
        free(actuel);
}

void running_status(int n, int pid, char *cmd, int sortie)
{
        char char_jobs[305];
        sprintf(char_jobs, "[%d]   %d        Running      %s\n", n + 1, pid, cmd);
        write(sortie, char_jobs, strlen(char_jobs));
}

void done_status(int n, int pid, char *cmd, int sortie)
{
        char char_jobs[300];
        sprintf(char_jobs, "[%d]   %d        Done      %s\n", n + 1, pid, cmd);
        write(sortie, char_jobs, strlen(char_jobs));
}

void killed_status(int n, int pid, char *cmd, int sortie)
{
        char char_jobs[302];
        sprintf(char_jobs, "[%d]   %d        Killed      %s\n", n + 1, pid, cmd);
        write(sortie, char_jobs, strlen(char_jobs));
}
void stopped_status(int n, int pid, char *cmd, int sortie)
{
        char char_jobs[305];
        sprintf(char_jobs, "[%d]   %d        Stopped      %s\n", n + 1, pid, cmd);
        write(sortie, char_jobs, strlen(char_jobs));
}


