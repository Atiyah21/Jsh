#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "commande.h"

static char **parseur(char *ligne, int *x)
{
        assert(ligne != NULL);
        assert(*ligne != '\0');

        size_t len = strlen(ligne);
        if (*(ligne + len - 1) == ' ')
                return NULL;

        size_t nb_mots = 1;

        for (size_t i = 0; i < len; ++i)
                if (*(ligne + i) == ' ')
                        ++nb_mots;
        *x = nb_mots;
        char **parseur = malloc(nb_mots * sizeof(char *));
        *parseur = strtok(ligne, " ");

        for (size_t i = 1; i < nb_mots; ++i)
                *(parseur + i) = strtok(NULL, " ");

        return parseur;
}

int main(int argc, char const *argv[])
{
        int x;
        //rl_initialize();
        //rl_outstream = stderr;
        while (1)
        {
                char *buf = readline(">");
                char **ligne = parseur(buf, &x);
                for (int i = 0; i < x; ++i)
                {
                        int res = execute_externe(ligne,x);
                }
        }
        return 0;
}