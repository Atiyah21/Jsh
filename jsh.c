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
#include <fcntl.h>
#include <errno.h>
#include "commandes.h"

typedef struct
{
    pid_t pid;          
    char command[256];  
    int status;         
} Job;

Job jobs[128];
int num_jobs = 0; 

static int ret = 0;
// char *s = NULL;

int pid;
static char prev_directory[PATH_MAX];
int fd=-1;

/*@return 0 si pas de redirection 
* -1 si erreur
* 1 si il y a une redirection 
*/
int redirection(char** ligne,int nbw){
  if(nbw==1) return 0;
  int tmp=0;
  for (int i=0;ligne[i]!=NULL;i++){
    if (strcmp(ligne[i],"<")==0){
      if(i+1>=nbw){
        fprintf(stderr,"jsh: Missing arguement\n");
        return -1;
      }else {
        fd=open(ligne[i+1],O_RDONLY);dup2(fd,STDIN_FILENO);
        ligne[i]=NULL;
          if(fd==-1){
            fprintf(stderr,"jsh: File doesn't exist\n");
            return -1;
          }
        tmp=1;
      }
    }else if (strcmp(ligne[i],">")==0){
      if(i+1>=nbw){
          fprintf(stderr,"jsh: Missing arguement\n");
          return -1;
        }else {
          fd=open(ligne[i+1],O_WRONLY|O_CREAT|O_EXCL,0666);dup2(fd,STDOUT_FILENO);
          ligne[i]=NULL;
          if(fd==-1){
            fprintf(stderr,"jsh: cannot overwrite existing file\n");
            return -1;
          }
          tmp= 1;
        }
    }else if (strcmp(ligne[i],">>")==0){
      if(i+1>=nbw){
        fprintf(stderr,"jsh: Missing arguement\n");
        return -1;
      }else{
        fd=open(ligne[i+1],O_WRONLY|O_CREAT|O_APPEND,0666);dup2(fd,STDOUT_FILENO);
        ligne[i]=NULL;
        if(fd==-1)return -1;
        tmp= 1;      
      }
    }else if (strcmp(ligne[i],">|")==0){
      if(i+1>=nbw){
          fprintf(stderr,"jsh: Missing arguement\n");
          return -1;
        }else {
          fd=open(ligne[i+1],O_WRONLY|O_CREAT|O_TRUNC,0666);dup2(fd,STDOUT_FILENO);
          ligne[i]=NULL;
          if(fd==-1){
            return -1;
          }
          tmp= 1;
        }
    }else if (strcmp(ligne[i],"2>")==0){
      if(i+1>=nbw){
        fprintf(stderr,"jsh: Missing arguement\n");
        return -1;
      }else{
          fd=open(ligne[i+1],O_WRONLY|O_CREAT|O_EXCL,0666);
          dup2(fd,STDERR_FILENO);
          ligne[i]=NULL;
          if(fd==-1){
            fprintf(stderr,"jsh: cannot overwrite existing file\n");
            return -1;
          }
          tmp= 1;
      }
    }else if (strcmp(ligne[i],"2>>")==0){
      if(i+1>=nbw){
        fprintf(stderr,"jsh: Missing arguement\n");
        return -1;
      }else{
          fd=open(ligne[i+1],O_WRONLY|O_CREAT|O_APPEND,0666);dup2(fd,STDERR_FILENO);
          ligne[i]=NULL;
          if(fd==-1){
            return -1;
          }
          tmp= 1;
      }
    }else if (strcmp(ligne[i],"2>|")==0){
      if(i+1>=nbw){
        fprintf(stderr,"jsh: Missing arguement\n");
        return -1;
      }else{
          fd=open(ligne[i+1],O_WRONLY|O_CREAT|O_TRUNC,0666);dup2(fd,STDERR_FILENO);
          ligne[i]=NULL;
          if(fd==-1){
            return -1;
          }
          tmp= 1;
      }
    }
  }
  return tmp;
}


int execute(int argc, char **argv)
{
  bool bg = false;

  if (argc > 1 && strcmp(argv[argc - 1], "&") == 0)
  {
    
    argv[argc - 1] = NULL;
    argc--;
    bg = true;
    pid = fork();
    if (pid < 0){
      perror("fork error");
      return 1;
    }
    else if (pid == 0){
      execvp(argv[0], argv);
      perror("execvp error");
      exit(EXIT_FAILURE);
    }
    else{
     if(bg){
      jobs[num_jobs].pid = pid;
      strcpy(jobs[num_jobs].command, argv[0]);
      for (int i = 1; i < argc; i++) {
        strcat(jobs[num_jobs].command, " ");
        strcat(jobs[num_jobs].command, argv[i]);
      }
      num_jobs++;
      printf("[%d]   %d        Running %s\n", num_jobs - 1, jobs[num_jobs - 1].pid, jobs[num_jobs - 1].command);
      return 0;
     }
     else{
      waitpid(pid, &ret, 0);
      return WEXITSTATUS(ret);
     }
    }
  }else{
  int tmp;
  int fd0 =dup(0),fd1 =dup(1),fd2 =dup(2);
  int i=redirection(argv,argc);
  if(i==-1) {
      return 1;
  }
  if (strcmp(argv[0], "pwd") == 0)
  {
    if (argc != 1)
    {
      printf("too many arguments\n");
      tmp= 1;
    }
    char *actuel = malloc(100);
    tmp= pwd(actuel, 1);
  }else if (strcmp(argv[0], "cd") == 0)
  {
    if (argc > 2)
    {
      printf("too many arguments\n");
      tmp= 1;
    }
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
  } else if (strcmp(argv[0], "?") == 0){
    if (argc != 1)
    {
      printf("too many arguments\n");
      tmp= 1;
    }
    printf("%d\n", ret);
    tmp= 0;
  }
  else if (strcmp(argv[0], "exit") == 0)
  {
    if (argc > 3)
    {
      printf("too many arguments\n");
      tmp= 1;
    }
    if (argc > 1)
    {
      tmp= atoi(argv[1]);
    }
    else
      tmp= ret;
  }else {switch (pid = fork())
  {
  case 0:
    execvp(argv[0], argv);
    exit(ret);//Normalement cette ligne ne s'execute jamais
  default:
    waitpid(pid, &ret, 0);
    tmp=WEXITSTATUS(ret);
  }}
  if(i==1){
    close(fd);
    dup2(fd0,0);
    dup2(fd1,1);
    dup2(fd2,2);
  }
  return tmp;
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
    for(int i=*nbw;i<lim;i++){
      res[i]=NULL;
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

    sprintf(pro, "\001\033[91m\002[%d]\001\033[36m\002...%s\001\033[00m\002$ ", num_jobs, (actuel + taille_chemin - 22));
  }
  else
  {
    sprintf(pro, "\001\033[91m\002[%d]\001\033[36m\002%s\001\033[00m\002$ ", num_jobs, actuel);
  }
  free(actuel);
}

int main(int argc, char const *argv[])
{
  rl_initialize();
  rl_outstream = stderr;
  char s[256];
  char *buf = NULL;
  int nbw=0;
  while (1)
  {
    start:
    prompt(s);
    if (buf != NULL)free(buf);

    buf = readline(s);
    add_history(buf);
    if (buf == NULL)
      break;
    if (*buf == '\0') goto start;

    char *ligne[128];
    char str[strlen(buf)+1];
    strcpy(str,buf);
    split(str, &nbw,ligne);

    if (ligne==NULL) goto start;
    /*Bouger execute dans redirection et pour faire les redirections, faire un fork excuter  */
    else if (strcmp(ligne[0], "exit") == 0)
    {
      if (nbw == 2)
        ret = atoi(ligne[1]);
      goto exit;
    }else{
      ret = execute(nbw, ligne);
      if(fd!=-1) 
        close(fd); 
      fd=-1;
    }

    while (num_jobs > 0)
    {
        pid_t p = waitpid(-1, NULL, WNOHANG);
        if (p > 0)
        {
           for (int i = 0; i < num_jobs; i++) {

            if (jobs[i].pid == p) {
              num_jobs--;
              printf("[%d]   %d        Done    %s\n", i, jobs[i].pid, jobs[i].command);
              memmove(&jobs[i], &jobs[i + 1], (num_jobs - i) * sizeof(Job));
              i--;
            }
           }

        }
        else
        {
            break; // Sortir de la boucle si aucun processus en arrière-plan n'est terminé
        }
    }

  
  }

  exit : 
    if (buf != NULL)
    {
      free(buf);
    }
  return ret;
}
