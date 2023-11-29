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

static int ret = 0;
static int running = 1; // 0 si on a terminé

static bool space_only(char *ligne) {
  for (int i = 0; i < strlen(ligne); ++i) {
    if (*(ligne + i) != ' ' && *(ligne + i) != '\0')
      return false;
  }
  return true;
}

static char **parceur(char *ligne, int *x) {
  assert(ligne != NULL);
  if (space_only(ligne)) {
    *x = 0;
    return NULL;
  }
  size_t len = strlen(ligne);
  size_t nb_mots = 1;
  if (*(ligne) == ' ')
    nb_mots = 0;
  for (size_t i = 0; i < len; ++i)
    if (*(ligne + i) == ' ' &&
        (*(ligne + i + 1) != ' ' && *(ligne + i + 1) != '\0')) {

      ++nb_mots;
    }
  *x = nb_mots;
  char **ligne_decoupe = malloc(nb_mots * sizeof(char *));
  *ligne_decoupe = strtok(ligne, " ");

  for (size_t i = 1; i < nb_mots; ++i)
    *(ligne_decoupe + i) = strtok(NULL, " ");

  return ligne_decoupe;
}

int pwd(char *actuel, int affiche) {// 0 pour afficher, 1 pour ne pas afficher 

  actuel = malloc(PATH_MAX);
  if (actuel == NULL) {
    perror("pwd: Erreur malloc");
    return 1;
  }

  if (getcwd(actuel, PATH_MAX) == NULL) {
    perror("pwd: Erreur getcwd");
    free(actuel);
    return 1;
  }
  if(affiche)
    printf("%s\n", actuel);
  free(actuel);
  return 0;
}

int cd(char *ref) {
  bool fini = false;
  char *actuel = malloc(100);
  pwd(actuel,0);
  char *destination = malloc(100);

  if (ref == NULL) {
    char *home = getenv("HOME");
    if (home == NULL) {
      perror("HOME error");
      return 1;
    }
    destination = home;
  } else if (strcmp("..", ref) == 0) {
    if (chdir("..") != 0) {
      perror("chdir error");
      return 1;
    }
    fini = true;
  } else if (strcmp("-", ref) == 0) {
    printf("test2");
    char *precedent = getenv("OLDPWD");
    if (precedent == NULL) {
      perror("OLDPWD error");
      return 1;
    }
    destination = precedent;
  }

  else {
    destination = ref;
  }

  if (chdir(destination) != 0 && !fini) {
    perror("chdir error");
    return 1;
  }

  if (setenv("OLDPWD", actuel, 1) != 0) {
    perror("maj OLDPWD");
    return 1;
  }
  return 0;
}

int execute(int argc, char *argv[]) {
  if (strcmp(argv[0], "pwd") == 0) {
    if (argc != 1) {
      printf("too many arguments\n");
      return 1;
    }
    char *actuel = malloc(100);
    return pwd(actuel,1);
  }

  if (strcmp(argv[0], "cd") == 0) {
    if (argc > 2) {
      printf("too many arguments\n");
      return 1;
    }
    int tmp;
    if (argc == 1)
      tmp = cd(NULL);
    else if (argc == 2)
      tmp = cd(argv[1]);
    return tmp;
  }

  if (strcmp(argv[0], "?") == 0) {
    if (argc != 1) {
      printf("too many arguments\n");
      return 1;
    }
    printf("la valeur de retour est %d \n", ret);
    return ret;
  }
  if (strcmp(argv[0], "exit") == 0) {
    if (argc > 3) {
      printf("too many arguments\n");
      return 1;
    }
    running = 0;
    if (argc > 1) {
      return atoi(argv[1]);
    } else
      return ret;
  }
  switch (fork()) {
  case 0:
    execvp(argv[0], argv + 1);
  default:
    return WEXITSTATUS(wait(NULL));
  }
}

int main(int argc, char const *argv[]) {
  int x;
  rl_initialize();
  // rl_outstream = stderr;
  while (running) {
    char *buf = readline(">");
    if (*buf != '\0') {
      char **ligne = parceur(buf, &x);
      ret = execute(x, ligne);
//      for (int i = 0; i < x; i++)
//        free(ligne[i]);
//      free(ligne);
    }
  }
  exit(ret);
  return 0;
}
