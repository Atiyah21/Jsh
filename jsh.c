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

char* pwd()
{

        char *actuel = malloc(PATH_MAX);
        if (actuel == NULL)
        {
                perror("pwd: Erreur malloc");
                return "";
        }

        if (getcwd(actuel, PATH_MAX) == NULL)
        {
                perror("pwd: Erreur getcwd");
                free(actuel);
                return "";
        }

        return actuel;
}

void cd(char *ref){
	bool fini = false;
	char *actuel = malloc(100);
	actuel = pwd();
	char *destination = malloc(100);

	if(strcmp(" ", ref) == 0){
		char *home = getenv("HOME");
		if(home == NULL){
			perror("HOME error");
			exit(EXIT_FAILURE);
		}
		destination = home;
	}	
	else if (strcmp("..", ref) == 0){
		if (chdir("..") != 0) {
            		perror("chdir error");
            		exit(EXIT_FAILURE);
        	}
		fini = true;
	}
	else if(strcmp("-", ref) == 0){
		printf("test2");
		char *precedent = getenv("OLDPWD");
		if(precedent == NULL){
			perror("OLDPWD error");
			exit(EXIT_FAILURE);
		}
		destination = precedent;
	}

	else{
		destination = ref;
	}


	if(chdir(destination) != 0 && !fini){
		perror("chdir error");
		exit(EXIT_FAILURE);
	}

	if(setenv("OLDPWD", actuel, 1) != 0){
		perror("maj OLDPWD");
		exit(EXIT_FAILURE);
	}

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
                                        printf("%s\n", pwd());
				else if(strcmp(*(ligne + i), "cd") == 0){
					cd(*(ligne + i + 1));
				}
                                else{
                                        printf("%s\n", *(ligne + i));
                        
				}
			}
                }
        }
        return 0;
}
