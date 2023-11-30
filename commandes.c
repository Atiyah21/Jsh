#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "commandes.h"


int pwd(char *actuel, int affiche) {// 0 pour afficher, 1 pour ne pas afficher 

  /*actuel = malloc(PATH_MAX);
  if (actuel == NULL) {
    perror("pwd: Erreur malloc");
    return 1;
  }*/

  if (getcwd(actuel, PATH_MAX) == NULL) {
    perror("pwd: Erreur getcwd");
    //free(actuel);
    return 1;
  }
  if(affiche)
    printf("%s\n", actuel);
  //free(actuel);
  return 0;
}

int cd(char *ref) {
  bool fini = false;
  char *actuel = malloc(100);
  pwd(actuel,0);
  char destination[100];

  if (ref == NULL) {
    char *home = getenv("HOME");
    if (home == NULL) {
      perror("HOME error");
      return 1;
    }
   strncpy(destination, home, sizeof(destination));
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
    strncpy(destination, precedent, sizeof(destination));
  }

  else {
	  strncpy(destination, ref, sizeof(destination));

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
