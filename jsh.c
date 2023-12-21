#include <linux/limits.h>
#include <assert.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdbool.h>
#include "commandes.h"
#include "redirection.h"
#include "affichage.h"

typedef struct
{
  //pid -1 veut dire que c'est vide
  pid_t pid;
  char command[256];
  /*
  1 -> Done
  0 -> en cours
  -1 -> Killed
  2 -> Sleeping? 
  */
  int status;
  int index;
} Job;

Job jobs[512];
int num_jobs = 0;
static int ret = 0;
// char *s = NULL;

int pid;
static char prev_directory[PATH_MAX];

void update_num_jobs()
{
  int res = 0;
  for (int i = 0; i < 128; i++)
  {
    if (jobs[i].pid!=-1)
      res++;
  }
  num_jobs = res;
}

int get_smallest_index(){
  for(int i = 0 ; i< 512 ; i++){
    if (jobs[i].status==-1) return i;
  }
  perror("No space left");
  return -1;
}

int execute(int argc, char **argv)
{
  if (argc > 1 && strcmp(argv[argc - 1], "&") == 0)
  {

    argv[argc - 1] = NULL;
    argc--;
    pid = fork();
    if (pid < 0)
    {
      perror("fork error");
      return 1;
    }
    else if (pid == 0)
    {

      execvp(argv[0], argv);
      perror("execvp error");
      exit(EXIT_FAILURE);
    }
    else
    {

      jobs[num_jobs].pid = pid;
      jobs[num_jobs].index = get_smallest_index() ;
      jobs[num_jobs].status = 1;
      strcpy(jobs[num_jobs].command, argv[0]);
      for (int i = 1; i < argc; i++)
      {
        strcat(jobs[num_jobs].command, " ");
        strcat(jobs[num_jobs].command, argv[i]);
      }
      num_jobs++;
      running_status(num_jobs, jobs[num_jobs - 1].pid, jobs[num_jobs - 1].command);
      return 0;
    }
  }
  else
  {
    int tmp;
    int fd0 = dup(0), fd1 = dup(1), fd2 = dup(2);
    int i = redirection(argv, argc);
    if (i == -1)
    {
      return 1;
    }
    if (strcmp(argv[0], "pwd") == 0)
    {
      if (argc != 1)
      {
        printf("too many arguments\n");
        tmp = 1;
      }
      char *actuel = malloc(100);
      tmp = pwd(actuel, 1);
    }

    else if (strcmp(argv[0], "cd") == 0)
    {
      if (argc > 2)
      {
        printf("too many arguments\n");
        tmp = 1;
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
    }
    else if (strcmp(argv[0], "?") == 0)
    {
      if (argc != 1)
      {
        printf("too many arguments\n");
        tmp = 1;
      }
      printf("%d\n", ret);
      tmp = 0;
    }
    else if (strcmp(argv[0], "exit") == 0)
    {
      if (argc > 3)
      {
        printf("too many arguments\n");
        tmp = 1;
      }
      if (argc > 1)
      {
        tmp = atoi(argv[1]);
      }
      else
        tmp = ret;
    } else if (strcmp(argv[0], "kill") == 0)
    {
    if (argv[1] != NULL && argv[1][0] == '%'){

                        if (strlen(argv[1]) >= 2){

                                int k = atoi(argv[1] + 1) - 1;

                                if (k >= 0 ){

                                        kill(jobs[k].pid, SIGTERM);
                                        if (jobs[k].status != 1){
                                                jobs[k].status = -1;
                                        }
                                }
                                else{
                                        printf("Invalid job index\n");
                                }
                        }
                        else{
                                printf("Invalid argument for kill\n");
                        }
                }
                else{
                        printf("Invalid syntax for kill\n");
                }

    }else if (strcmp(argv[0], "jobs") == 0)
    {
      for (int i = 0; i < num_jobs; i++)
      {
        if (jobs[i].status == -1)
          killed_status(i, jobs[i].pid, jobs[i].command);
        if (jobs[i].status == 0)
          running_status(i, jobs[i].pid, jobs[i].command);
        if (jobs[i].status == 1)
          done_status(i, jobs[i].pid, jobs[i].command);
      }
      tmp = 0;
    }
    else
    {
      switch (pid = fork())
      {
      case 0:
        execvp(argv[0], argv);
        exit(ret); // Normalement cette ligne ne s'execute jamais
      default:
        waitpid(pid, &ret, 0);
        tmp = WEXITSTATUS(ret);
      }
    }
    if (i == 1)
    {
      close(fd);
      dup2(fd0, 0);
      dup2(fd1, 1);
      dup2(fd2, 2);
    }
    return tmp;
  }
}

void empty(int i){
  jobs[i].status=-1;
  jobs[i].index=-1;
  jobs[i].command[0]='\0';
}

int split(char *str, int *nbw, char **res)
{
  *nbw = 0;
  int lim = 128;
  char *token = strtok(str, " ");
  while (token != NULL)
  {
    if (*nbw == lim)
    {
      printf("Erreur prompt trop long");
      return -1;
    }
    res[*nbw] = token;
    token = strtok(NULL, " ");
    (*nbw) = (*nbw) + 1;
  }
  for (int i = *nbw; i < lim; i++)
  {
    res[i] = NULL;
  }
  return 0;
}

int main(int argc, char const *argv[])
{
  rl_initialize();
  rl_outstream = stderr;
  char s[256];
  char *buf = NULL;
  int nbw = 0;
  while (1)
  {
  start:
    prompt(s, num_jobs);
    if (buf != NULL)
      free(buf);

    buf = readline(s);
    add_history(buf);
    if (buf == NULL)
      break;
    if (*buf != '\0')
    {

      char *ligne[128];
      char str[strlen(buf) + 1];
      strcpy(str, buf);
      split(str, &nbw, ligne);

      if (ligne == NULL)
        goto start;
      else if (strcmp(ligne[0], "exit") == 0)
      {
        if (nbw == 2)
          ret = atoi(ligne[1]);
        goto exit;
      }
      else
      {
        ret = execute(nbw, ligne);
        if (fd != -1)
          close(fd);
        fd = -1;
      }
    }
      while (num_jobs > 0){
      pid_t p = waitpid(-1, NULL, WNOHANG);
      if (p > 0){
         for (int i = 0; i < 512; i++) {
          if (jobs[i].pid == p) {
            if(jobs[i].status == -1 )
              fprintf(stderr, "[%d]   %d       Killed  %s\n", i, jobs[i].pid, jobs[i].command);
            else if(jobs[i].status == 1 )
              fprintf(stderr, "[%d]   %d       Done    %s\n", i, jobs[i].pid, jobs[i].command);
            num_jobs--;
            empty(i);
            
          }
         }
      }
      else
          break;
    }
  }

exit:
  if (buf != NULL)
  {
    free(buf);
  }
  return ret;
}
