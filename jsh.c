#include <linux/limits.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "commandes.h"

static int ret = 0;
static char prev_directory[PATH_MAX];

bool space_only(char *ligne) {
  for (int i = 0; i < strlen(ligne); ++i) {
    if (*(ligne + i) != ' ' && *(ligne + i) != '\0')
      return false;
  }
  return true;
}



char *del_space(char *str) {
  char *nv = NULL;
  size_t len = strlen(str);
  size_t deb = 0;
  int fin = len - 1;

  while (isspace(str[deb]) && str[deb] != '\0' && deb < len)
    deb++;
  while (fin >= 0 && isspace(str[fin]))
    fin--;

  if (fin == -1) {
    nv = malloc(sizeof(char));
    assert(nv != NULL); // On vérifie que l'allocation s'est bien passée.
    nv[0] = '\0';
    return nv;
  }

  nv = malloc(sizeof(char) * (fin - deb + 2));
  assert(nv != NULL); // On vérifie que l'allocation s'est bien passée.
  int i;
  for (i = 0; i <= fin - deb; i++)
    nv[i] = str[deb + i];
  nv[i] = '\0';
  return nv;
}

int nbwords(char *str) {
  int c = 0; // Compteur du nombre de mots.
  bool mot = false;
  for (int i = 0; str[i] != '\0'; i++) {
    if (!isspace(str[i])) {
      if (!mot) // Si on ne regarde pas déjà un mot, alors on incrémente le
                // compteur de mots.
      {
        mot = true;
        c++;
      }
    } else
      mot = false;
  }
  return c;
}

char *next(char *str) {
  if (str == NULL) // Si la chaine 'str' est NULL
    return NULL;
  size_t i = 0;
  while (!(isspace(str[i]) || str[i] == '\0'))
    ++i;
  char *t = malloc(sizeof(char) * i + 1);
  assert(t != NULL); // On vérifie que l'allocation s'est bien passée.
  if (i == strlen(str))
    return strcpy(t, str);
  memcpy(t, str, i);
  t[i] = '\0';
  return t;
}

char **split(char *ligne, int *nbw) {
  if(space_only(ligne)) return NULL;
  char *nvligne = del_space(ligne); // on efface les espaces
  *nbw = nbwords(nvligne);          // nombre de mots
  char **tmp = malloc((*nbw) * sizeof(char *)+1);
  if (nvligne == NULL) {
    puts("Erreur : L'argument est NULL.");
    exit(-1);
  }
  int i = 0;
  for (int j = 0; j < *nbw; j++) {
    tmp[j] = next(nvligne + i);
    i = i + (strlen(tmp[j]) + 1);
  }
  tmp[*nbw]=NULL;
  free(nvligne);
  return tmp;
}



int execute(int argc, char **argv) {
  if (strcmp(argv[0], "pwd") == 0) {
    if (argc != 1) {
      printf("too many arguments\n");
      return 1;
    }
    char *actuel = malloc(100);
    return pwd(actuel, 1);
  }

  if (strcmp(argv[0], "cd") == 0) {
    if (argc > 2) {
      printf("too many arguments\n");
      return 1;
    }
    int tmp;
    if (argc == 1){
      getcwd(prev_directory, PATH_MAX);
      tmp = cd(NULL);
    }else if (argc == 2){
      if(strcmp(argv[1],"-")==0){
        tmp = cd(prev_directory);
        getcwd(prev_directory, PATH_MAX);
      }else if (strcmp(argv[1],"$HOME")==0){
        getcwd(prev_directory, PATH_MAX);
        tmp = cd(NULL);
      }else{ 
        getcwd(prev_directory, PATH_MAX);
        tmp = cd(argv[1]);  
      }
    }
    return tmp;
  }

  if (strcmp(argv[0], "?") == 0) {
    if (argc != 1) {
      printf("too many arguments\n");
      return 1;
    }
    printf("%d\n", ret);
    return 0;
  }
  if (strcmp(argv[0], "exit") == 0) {
    if (argc > 3) {
      printf("too many arguments\n");
      return 1;
    }
    if (argc > 1) {
      return atoi(argv[1]);
    } else
      return ret;
  }
  switch (fork()) {
  case 0:
    execvp(argv[0],argv);
  default:;
    int x;
    wait(&x);
    return WEXITSTATUS(x);
  }
}

char *prompt() {
  char *s = malloc(30 * sizeof(char));
  char *actuel = malloc(PATH_MAX);
  getcwd(actuel, PATH_MAX);
  size_t taille_chemin = strlen(actuel);
  s[0] = '\0';
  strcat(s, "\033[91m[0]\033[36m");
  if (taille_chemin > 25) {
    strcat(s, "...");
    strcat(s, (actuel + taille_chemin - 22));
  } else {
    strcat(s, actuel);
  }
  strcat(s, "\033[00m$ ");
  free(actuel);
  return s;
}

int main(int argc, char const *argv[]) {
  rl_initialize();
  rl_outstream = stderr;
  while (1) {
    char *s = prompt();
    char *buf = readline(s);
    add_history(buf);
    free(s);
    if (buf == NULL)
      exit(0);
    if (*buf != '\0') {
      int nbw;
      char **ligne = split(buf, &nbw);
      if (ligne != NULL) {
        if (strcmp(ligne[0], "exit")==0) {
          if (nbw == 2)
            ret = atoi(ligne[1]);
          for (int i = 0; i < nbw; i++)
            free(ligne[i]);
          free(ligne);
          break;
        }
        ret = execute(nbw, ligne);
        //  printf("%d\n",ret);
        for (int i = 0; i < nbw+1; i++)
          free(ligne[i]);
        free(ligne);
      }
            
    }
  }

}
