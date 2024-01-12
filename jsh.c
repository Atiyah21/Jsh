#include <linux/limits.h>
#include <assert.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "commandes.h"
#include "redirection.h"
#include <signal.h>
#include "affichage.h"

int execute(int argc, char **argv);
int get_job_id(int k);
void empty(int i);
void show_status(int i, int sortie);

typedef struct
{
  // pid -1 veut dire que c'est vide
  pid_t pid;
  char command[256];
  /*
  1 -> Done
  0 -> en cours
  -1 -> Killed
  2 -> suspendu*/
  int status;
  int index;
} Job;

Job jobs[512];
int num_jobs = 0;
static int ret = 0;
// char *s = NULL;

int pid;
static char prev_directory[PATH_MAX];

void fg(int job)
{
  int job_index = get_job_id(job - 1);
  pid_t pid = jobs[job_index].pid;
  if(jobs[job_index].status == 2)
    if (kill(pid, SIGCONT) == -1)
      perror("Erreur fg: SIGCONT");
  int status;
  tcsetpgrp(STDIN_FILENO, getpgid(pid));
  waitpid(pid, &status, WUNTRACED);
  tcsetpgrp(STDIN_FILENO, getpid());
  // struct sigaction action;
  // memset(&action, 0, sizeof(struct sigaction));
  if (WIFSTOPPED(status)){
    jobs[job_index].status = 2; // suspendu
    show_status(job_index, 2);
    return;
  }
  jobs[job_index].status = (WIFEXITED(status)) ? 1 : -1; // Done ou Killed
  // show_status(job_index, 2);
  num_jobs--;
  empty(job_index);
}

void bg(int job)
{
  int job_index = get_job_id(job - 1);

  pid_t pid = jobs[job_index].pid;

  if (kill(pid, SIGCONT) == -1)
  {
    perror("Erreur bg: SIGCONT");
    return;
  }
  jobs[job_index].status=0;
}

void update_num_jobs()
{
  int res = 0;
  for (int i = 0; i < 512; i++)
  {
    if (jobs[i].index != -1)
    {
      res++;
    }
  }
  num_jobs = res;
}

int get_smallest_index()
{
  for (int i = 0; i < 512; i++)
  {
    if (jobs[i].index == -1)
      return i;
  }
  perror("No space left");
  return -1;
}

int get_job_id(int k)
{
  for (int i = 0; i < 512; i++)
  {
    if (jobs[i].index == k)
      return i;
  }
  return -1;
}

int get_job_pid(pid_t k)
{
  for (int i = 0; i < 512; i++)
  {
    if (jobs[i].pid == k)
      return i;
  }
  return -1;
}

void add_job(int argc, char **argv, pid_t pid, int st)
{
  Job j;
  j.pid = pid;
  j.index = get_smallest_index();
  j.status = st;
  strcpy(j.command, argv[0]);
  int i = 1;
  for (; argv[i] != NULL; i++)
  {
    strcat(j.command, " ");
    strcat(j.command, argv[i]);
  }
  jobs[num_jobs] = j;
  num_jobs++;
}

void empty(int i)
{
  Job j = {
      .status = -2,
      .index = -1,
      .command = {'\0'}};
  jobs[i] = j;
}

void show_status(int i, int sortie)
{
  if (jobs[i].status == -1)
    killed_status(i, jobs[i].pid, jobs[i].command, sortie);
  else if (jobs[i].status == 1)
    done_status(i, jobs[i].pid, jobs[i].command, sortie);
  else if (jobs[i].status == 0)
    running_status(i, jobs[i].pid, jobs[i].command, sortie);
  else if (jobs[i].status == 2)
    stopped_status(i, jobs[i].pid, jobs[i].command, sortie);
}

int command_pipe(char **ligne, int nbw)
{
  int tmp = 0;
  for (int i = 0; ligne[i] != NULL; i++)
  {
    if (strcmp(ligne[i], "|") == 0)
    {
      if (i + 1 >= nbw)
      {
        fprintf(stderr, "jsh: Missing arguement\n"); // TODO changer en write sur 2
        return -1;
      }
      else
      {
        int stin = dup(0);
        int stout = dup(1);
        int fd[2];
        pipe(fd);
        int pid = fork();
        if (pid == 0)
        {
          close(fd[0]);
          dup2(fd[1], STDOUT_FILENO);
          ligne[i] = NULL;
          execute(i, ligne);
          close(fd[1]);
          exit(0);
        }
        else
        {
          close(fd[1]);
          dup2(fd[0], STDIN_FILENO);
          // ligne = ligne + i + 1;
          waitpid(pid, &ret, -1);
          tmp = execute(nbw - i - 1, ligne + i + 1);
          close(fd[0]);
          dup2(stin, 0);
          dup2(stout, 1);
          return tmp;
        }
      }
    }
  }
  return -1;
}

int process_substitution(int nbw, char **ligne)
{
  int tmp = 0;
  for (int i = 0; ligne[i] != NULL; i++)
  {
    if (strcmp(ligne[i], "<(") == 0)
    {
      int j = next(ligne + i);
      if (j == -1)
      {
        fprintf(stderr, "jsh: Missing arguements\n");
        return -1;
      }
      else
      {
        // if(i==-1){
        //     fprintf(stderr, "jsh: Panthesis not closed\n");
        //     return -1;
        // }
        // int e=0;
        char c[20];
        sprintf(c, "/tmp/tube%d", getpid());
        mkfifo(c,0666);// On cree un tube avec mkfifo | O_NONBLOCK
        int file = open(c, O_RDWR | O_NONBLOCK );
        if (file == -1)
        {
          fprintf(stderr, "jsh: Process substitution error\n"); // TODO changer en write sur 2
          return -1;
        }
        int stin = dup(STDIN_FILENO);
        int stout = dup(STDOUT_FILENO);
        dup2(file, STDOUT_FILENO);
        ligne[i] = NULL;
        ligne[i + j] = NULL;
        // int pid=fork();
        // if(pid==0){
        // printf("%s\n", ligne[i + 1]);
        execute(j - i, ligne + i + 1); // TODO segmentation ici
        close(file);
        dup2(stin, STDIN_FILENO);
        dup2(stout, STDOUT_FILENO);
        close(stin);
        close(stout);
        //   exit(0);
        // }else{
        // waitpid(pid, &e, -1);
        for (int y = i; y < j + i + 1; y++)
          ligne[y] = NULL;
        char *nvligne[128];
        int cpt = 0;
        for (int s = 0; s < nbw; s++)
        { // on parcourt ligne,
          if (s == i)
          {
            //char str1[128];
            //strcpy(str1, c); // nom du fichier créé
            nvligne[cpt] = c;
            cpt++;
          }
          else if (ligne[s] != NULL)
          {
            //char str2[128];
            //strcpy(str2, ligne[s]);
            nvligne[cpt] = ligne[s];
            cpt++;
          }
        }
        nvligne[cpt] = NULL;
        tmp = execute(cpt, nvligne);
        unlink(c);
        return tmp;
        // }
      }
    }
  }
  return -1;
}

int execute(int argc, char **argv)
{
  int p_sub = process_substitution(argc, argv);
  if (p_sub != -1)
    return p_sub;
  int pip = command_pipe(argv, argc);
  if (pip != -1)
    return pip;
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
  }
  else if (strcmp(argv[0], "fg") == 0)
  {
    fg(atoi(argv[1] + 1));
  }
  else if (strcmp(argv[0], "bg") == 0)
  {
    bg(atoi(argv[1] + 1));
  }
  else if (strcmp(argv[0], "kill") == 0)
  {
    if (argv[1] != NULL)
    {
      if (argv[1][0] == '%')
      {
        int k = atoi(argv[1] + 1) - 1;
        int i = get_job_id(k);
        if (i == -1)
        {
          printf("erreur pas de processus %d\n", k);
          tmp = 1;
          goto end;
        }
        kill(-jobs[i].pid, 15);
        // jobs[i].status = -1;
        tmp = 0;
      }
      else if (argv[1][0] == '-')
      {
        int sig = atoi(argv[1] + 1);
        if (argv[2][0] == '%')
        {
          int k = atoi(argv[2] + 1) - 1;
          int i = get_job_id(k);
          if (i == -1)
          {
            printf("erreur pas de processus %d\n", k);
            tmp = 1;
            goto end;
          }
          kill(-jobs[i].pid, sig);
          // if (sig == 20)
          //   jobs[i].status = 2;
          // else if (sig == 18)
          //   jobs[i].status = 0;
          // else
          //   jobs[i].status = -1;
          tmp = 0;
        }
        else
        {
          pid_t k = atoi(argv[2] + 1) - 1;
          int i = get_job_pid(k);
          if (i == -1)
          {
            printf("erreur pas de processus %d\n", k);
            tmp = 1;
            goto end;
          }
          kill(jobs[i].pid, sig);
          // if (sig == 20)
          //   jobs[i].status = 2;
          // else if (sig == 18)
          //   jobs[i].status = 0;
          // else
          //   jobs[i].status = -1;
          tmp = 0;
        }
      }
      else
      {
        int k = atoi(argv[1]);
        int i = get_job_pid(k);
        if (i == -1)
        {
          printf("erreur pas de processus %d\n", k);
          tmp = 1;
          goto end;
        }
        kill(jobs[i].pid, SIGTERM);
        // jobs[i].status = -1;
        tmp = 0;
      }
    }
  }
  else if (strcmp(argv[0], "jobs") == 0)
  {
    while (num_jobs > 0)
    {
      int tmp;
      pid_t p = waitpid(-1, &tmp, WNOHANG | WUNTRACED | WCONTINUED);
      if (p > 0)
      {
        int i = get_job_pid(p);
        if (i != -1)
        {
          if (WIFCONTINUED(tmp))
          {
            jobs[i].status = 0;
          }
          else if (WIFSTOPPED(tmp))
          {
            jobs[i].status = 2;
          }
          else if (WIFEXITED(tmp))
          {
            jobs[i].status = 1;
          }
          else
          {
            jobs[i].status = -1;
          }
        }
      }
      else
        break;
    }
    for (int i = 0; i < 512; i++)
    {
      if (jobs[i].index != -1)
        show_status(i, 1);
      if (jobs[i].status == 1 || jobs[i].status == -1)
      {
        empty(i);
        num_jobs--;
      }
    }
    tmp = 0;
  }
  else if (argc > 1 && strcmp(argv[argc - 1], "&") == 0)
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
      struct sigaction action;
      memset(&action, 0, sizeof(struct sigaction));
      action.sa_handler = SIG_DFL; // Set handler to default
      sigaction(SIGINT, &action, NULL);
      sigaction(SIGTERM, &action, NULL);
      sigaction(SIGTTIN, &action, NULL);
      sigaction(SIGQUIT, &action, NULL);
      sigaction(SIGTTOU, &action, NULL);
      sigaction(SIGTSTP, &action, NULL);
      sigaction(SIGSTOP, &action, NULL);
      pid = getpid();
      setpgid(pid, pid);
      execvp(argv[0], argv);
      perror("execvp error");
      exit(EXIT_FAILURE);
    }
    else
    {
      Job j;
      j.pid = pid;
      j.index = get_smallest_index();
      j.status = 0;
      strcpy(j.command, argv[0]);
      for (int i = 1; i < argc; i++)
      {
        strcat(j.command, " ");
        strcat(j.command, argv[i]);
      }
      jobs[j.index] = j;
      num_jobs++;
      running_status(j.index, jobs[j.index].pid, jobs[j.index].command, 2);
      return 0;
    }
  }
  else
  {
    switch (pid = fork())
    {
    case 0:;
      struct sigaction action;
      memset(&action, 0, sizeof(struct sigaction));
      action.sa_handler = SIG_DFL; // Set handler to default
      sigaction(SIGINT, &action, NULL);
      sigaction(SIGTERM, &action, NULL);
      sigaction(SIGTTIN, &action, NULL);
      sigaction(SIGQUIT, &action, NULL);
      // sigaction(SIGTTOU, &action, NULL);
      sigaction(SIGTSTP, &action, NULL);
      sigaction(SIGSTOP, &action, NULL);
      int p =getpid();
      setpgid(p,p);
      tcsetpgrp(STDIN_FILENO, p);
      execvp(argv[0], argv);
      exit(ret); // Normalement cette ligne ne s'execute jamais
    default:
      waitpid(pid, &ret, WUNTRACED);
      tcsetpgrp(STDIN_FILENO, getpid());
      if (WIFSTOPPED(ret))
      {
        tmp = 148;
        if (i == 1)
        {
          argc = 0;
          while (argv[argc] != NULL)
            argc++;
        }
        int index = get_smallest_index();
        add_job(argc, argv, pid, 2);
        jobs[index].status = 2;
        show_status(index, 2);
      }
      else
      {
        tmp = WEXITSTATUS(ret);
      }
      break;
    }
  }
end:
  if (i == 1 || pip == 1)
  {
    close(fd);
    dup2(fd0, 0);
    dup2(fd1, 1);
    dup2(fd2, 2);
  }
  return tmp;
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

void init()
{
  for (size_t i = 0; i < 512; i++)
  {
    empty(i);
  }
}

void update_jobs()
{
  while (num_jobs > 0)
  {
    int tmp;
    pid_t p = waitpid(-1, &tmp, WNOHANG | WUNTRACED | WCONTINUED);
    if (p > 0)
    {
      int i = get_job_pid(p);
      if (i != -1)
      {
        if (WIFCONTINUED(tmp))
        {
          jobs[i].status = 0;
          show_status(i, 2);
        }
        else if (WIFSTOPPED(tmp))
        {
          jobs[i].status = 2;
          show_status(i, 2);
        }
        else if (WIFEXITED(tmp))
        {
          jobs[i].status = 1;
          show_status(i, 2);
          num_jobs--;
          empty(i);
        }
        else
        {
          jobs[i].status = -1;
          show_status(i, 2);
          num_jobs--;
          empty(i);
        }
      }
    }
    else
      break;
  }
}

int main(int argc, char const *argv[])
{

  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = SIG_IGN;
  sigaction(SIGINT, &action, NULL);
  sigaction(SIGTERM, &action, NULL);
  sigaction(SIGTTIN, &action, NULL);
  sigaction(SIGQUIT, &action, NULL);
  sigaction(SIGTTOU, &action, NULL);
  sigaction(SIGTSTP, &action, NULL);

  init();
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
        goto start; // continue;
      else if (strcmp(ligne[0], "exit") == 0)
      {
        if (num_jobs != 0)
        {
          fprintf(stderr, "Il y a des jobs en cours\n");
          ret = 1;
          goto jobs;
        }
        else if (nbw == 2)
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
  jobs:
    update_jobs();
  }

exit:
  if (buf != NULL)
  {
    free(buf);
  }
  return ret;
}
