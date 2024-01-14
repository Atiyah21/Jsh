#include "affichage.h"

void prompt(char *pro, int num_jobs)
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

void running_status(int n, int pid, char *cmd, int sortie)
{
        char char_jobs[305];
        sprintf(char_jobs, "[%d]   %d        Running      %s\n", n + 1, pid, cmd);
        write(sortie, char_jobs, strlen(char_jobs));
}

void done_status(int n, int pid, char *cmd, int sortie)
{
        char char_jobs[300];
        sprintf(char_jobs, "[%d]   %d        Done      %s\n", n + 1, pid, cmd);
        write(sortie, char_jobs, strlen(char_jobs));
}

void killed_status(int n, int pid, char *cmd, int sortie)
{
        char char_jobs[302];
        sprintf(char_jobs, "[%d]   %d        Killed      %s\n", n + 1, pid, cmd);
        write(sortie, char_jobs, strlen(char_jobs));
}
void stopped_status(int n, int pid, char *cmd, int sortie)
{
        char char_jobs[305];
        sprintf(char_jobs, "[%d]   %d        Stopped      %s\n", n + 1, pid, cmd);
        write(sortie, char_jobs, strlen(char_jobs));
}

int printchildren_process_id(int pid, int space){
  char p[PATH_MAX];
  sprintf(p,"/proc/%d/task/%d/children",pid,pid);
  int fd =open(p,O_RDONLY);
  if(fd==-1){
    perror("jsh: jobs -t Can't open children file");
    return -1;
  }
  char b[1024];
  int i =read(fd,b,1024);
  close(fd);
  if(i==-1){
    perror("jsh: jobs -t Can't read children file");
    return -1;
  }
  if(i==0) {
    return 0;
  }
  b[i]='\0';
  char *r[128] ;
  int n=0;
  split(b,&n,r);
  for(int i=0;i<n;i++){
    for (int j= 0; j < space; j++) {
        printf(" ");
    }
    printf("%s ",r[i]);
    char str2[PATH_MAX];
    sprintf(p,"/proc/%d/stat",atoi(r[i]));
    fd=open(p,O_RDONLY);
    read(fd,str2,PATH_MAX);
    strtok(str2," ");
    strtok(NULL," ");//nom
    strtok(NULL," ");//
    char *state=strtok(NULL," ");
    if(strcmp(state,"T")==0)
      printf("Stopped ");
    else if(strcmp(state,"Z")==0)
      printf("Zombie ");
    else if(strcmp(state,"R"))
      printf("Running ");
    else if(strcmp(state,"S"))
      printf("Sleeping ");
    else if(strcmp(state,"X"))
      printf("Killed ");
    else
      printf("Unknown ");

    char str[PATH_MAX];
    sprintf(p,"/proc/%d/cmdline",atoi(r[i]));
    int fd =open(p,O_RDONLY);
    read(fd,str,PATH_MAX);
    close(fd);
    printf("%s \n",str);

    // printchildren_process_id(atoi(r[i]),space+2);
  }
  return 0;
}