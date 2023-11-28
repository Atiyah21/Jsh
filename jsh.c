#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <limits.h>
#include <readline/readline.h>
#include <readline/history.h>

static bool space_only(char *ligne)
{
        for (int i = 0; i < strlen(ligne); ++i)
        {
                if (*(ligne + i) != ' ' && *(ligne + i) != '\0')
                        return false;
        }
        return true;
}

static char **parceur(char *ligne, int *x)
{
        assert(ligne != NULL);
        if (space_only(ligne))
        {
                *x = 0;
                return NULL;
        }
        size_t len = strlen(ligne);
        size_t nb_mots = 1;
        if (*(ligne) ==' ')nb_mots = 0; 
        for (size_t i = 0; i < len; ++i)
                if (*(ligne + i) == ' ' && (*(ligne + i + 1) != ' ' && *(ligne + i + 1) != '\0'))
                {

                        ++nb_mots;
                }
        *x = nb_mots;
        char **ligne_decoupe = malloc(nb_mots * sizeof(char *));
        *ligne_decoupe = strtok(ligne, " ");

        for (size_t i = 1; i < nb_mots; ++i)
                *(ligne_decoupe + i) = strtok(NULL, " ");

        return ligne_decoupe;
}

int pwd()
{

        char *actuel = malloc(PATH_MAX);
        if (actuel == NULL)
        {
                perror("pwd: Erreur malloc");
                return 1;
        }

        if (getcwd(actuel, PATH_MAX) == NULL)
        {
                perror("pwd: Erreur getcwd");
                free(actuel);
                return 1;
        }

        printf("%s\n", actuel);
        free(actuel);
        return 0;
}

int main(int argc, char const *argv[])
{
        int x;
        rl_initialize();
        // rl_outstream = stderr;
        while (1)
        {
                char *buf = readline(">");
                if (*buf != '\0')
                {
                        char **ligne = parceur(buf, &x);
                        for (int i = 0; i < x; ++i)
                        {
                                if (strcmp(*(ligne + i), "pwd") == 0)
                                        pwd();
                                else
                                        printf("%s\n", *(ligne + i));
                        }
                }
        }
        return 0;
}
