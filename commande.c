#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

#include <sys/wait.h>

int execute_externe(char** argv, int argc){                  
    switch(fork()){
        case 0 : execvp(argv[0],argv+1);
        default : wait(NULL);
    }
}
