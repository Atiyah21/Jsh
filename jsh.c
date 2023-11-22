#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <readline/readline.h>
#include <readline/history.h>

static char **decoupe_chemin(char *chemin, int *x)
{
        assert(chemin != NULL);
        assert(*chemin != '\0');

        size_t len = strlen(chemin);
        if (*(chemin + len - 1) == ' ')
                return NULL;

        size_t nb_mots = 1;

        for (size_t i = 0; i < len; ++i)
                if (*(chemin + i) == ' ')
                        ++nb_mots;
        *x = nb_mots;
        char **chemin_decoupe = malloc(nb_mots * sizeof(char *));
        *chemin_decoupe = strtok(chemin, " ");

        for (size_t i = 1; i < nb_mots; ++i)
                *(chemin_decoupe + i) = strtok(NULL, " ");

        return chemin_decoupe;
}

int main(int argc, char const *argv[])
{
        int x;
        rl_initialize();
        //rl_outstream = stderr;
        while (1)
        {
                char *buf = readline(">");
                char **ligne = decoupe_chemin(buf, &x);
                for (int i = 0; i < x; ++i)
                {
                        printf("%s\n", *(ligne + i));
                }
        }
        return 0;
}