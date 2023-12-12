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
// char *s = NULL;
int pid;
static char prev_directory[PATH_MAX];


int execute(int argc, char **argv)
{
  if (strcmp(argv[0], "pwd") == 0)
  {
    if (argc != 1)
    {
      printf("too many arguments\n");
      return 1;
    }
    char *actuel = malloc(100);
    return pwd(actuel, 1);
  }

  if (strcmp(argv[0], "cd") == 0)
  {
    if (argc > 2)
    {
      printf("too many arguments\n");
      return 1;
    }
    int tmp;
    if (argc == 1)
    {
      getcwd(prev_directory, PATH_MAX);
      tmp = cd(NULL);
    }
    else if (argc == 2)
    {
      if (strcmp(argv[1], "-") == 0)
      {
        tmp = cd(prev_directory);
        getcwd(prev_directory, PATH_MAX);
      }
      else if (strcmp(argv[1], "HOME") == 0)
      {
        getcwd(prev_directory, PATH_MAX);
        tmp = cd(NULL);
      }
      else
      {
        getcwd(prev_directory, PATH_MAX);
        tmp = cd(argv[1]);
      }
    }
    return tmp;
  }

  if (strcmp(argv[0], "?") == 0)
  {
    if (argc != 1)
    {
      printf("too many arguments\n");
      return 1;
    }
    printf("%d\n", ret);
    return 0;
  }
  if (strcmp(argv[0], "exit") == 0)
  {
    if (argc > 3)
    {
      printf("too many arguments\n");
      return 1;
    }
    if (argc > 1)
    {
      return atoi(argv[1]);
    }
    else
      return ret;
  }
  switch (pid = fork())
  {
  case 0:
    execvp(argv[0], argv);
    exit(ret);
  default:
    waitpid(pid, &ret, 0);
    return WEXITSTATUS(ret);
  }
}

int split(char* str,int* nbw,char ** res){
    *nbw=0;

    int lim=128;
    char *token=strtok(str, " ");
    while (token!=NULL) {
      if(*nbw==lim){
        printf("Erreur prompt trop long");
        return -1;
      }
        res[*nbw]=token;
        token = strtok(NULL, " ");
        (*nbw)=(*nbw)+1;
    }
    res[*nbw] = NULL;
    return 0;
}

int redirection(char** ligne){
  // int i=0;
  for (int i=0;ligne[i]!=NULL;i++){
    if (strcmp(ligne[i],">")==0){
      if(ligne[i+1]==NULL){
        printf("Missing arguement");
        return -1;
      }
    }else if (strcmp(ligne[i],"<")==0){
      if(ligne[i+1]==NULL){
        printf("Missing arguement");
        return -1;
      }
    }else if (strcmp(ligne[i],">>")==0){
      if(ligne[i+1]==NULL){
        printf("Missing arguement");
        return -1;
      }
    }else if (strcmp(ligne[i],"2>")==0){
      if(ligne[i+1]==NULL){
        printf("Missing arguement");
        return -1;
      }
    }else if (strcmp(ligne[i],"2>>")==0){
      if(ligne[i+1]==NULL){
        printf("Missing arguement");
        return -1;
      }
    }else if (strcmp(ligne[i],"2>|")==0){
      if(ligne[i+1]==NULL){
        printf("Missing arguement");
        return -1;
      }
    }else i++;
  }
  return 0;
}

void prompt(char *pro)
{
  char *actuel = malloc(PATH_MAX);
  getcwd(actuel, PATH_MAX);
  size_t taille_chemin = strlen(actuel);
  // s[0] = '\0';
  if (taille_chemin > 25)
  {

    sprintf(pro, "\001\033[91m\002[%d]\001\033[36m\002...%s\001\033[00m\002$ ", 0, (actuel + taille_chemin - 22));
  }
  else
  {
    sprintf(pro, "\001\033[91m\002[%d]\001\033[36m\002%s\001\033[00m\002$ ", 0, actuel);
  }
  free(actuel);
}

int main(int argc, char const *argv[])
{
  rl_initialize();
  rl_outstream = stderr;
  char s[256];
  // = malloc(256);
  char *buf = NULL;
  // char **ligne=calloc(128, sizeof(char*));
  int nbw=0;
  while (1)
  {
    prompt(s);
    if (buf != NULL)
    {
      free(buf);
    }
    buf = readline(s);
    add_history(buf);
    if (buf == NULL)
      break;
    if (*buf != '\0')
    {
      char *ligne[128];
      char str[strlen(buf)+1];
      strcpy(str,buf);
      split(str, &nbw,ligne);
      // if(nbw > 1)
      //   redirection(ligne);
      if (ligne != NULL)
      {
        if (strcmp(ligne[0], "exit") == 0)
        {
          if (nbw == 2)
            ret = atoi(ligne[1]);
          goto exit;
        }else{
          ret = execute(nbw, ligne);
        }
      }
    }
  }

  exit : 
    if (buf != NULL)
    {
      free(buf);
    }
  // free(s);
  return ret;
}
