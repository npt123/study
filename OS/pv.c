#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t i, j;
    i = fork();
    if (i != 0) {
        j = fork();
    }
    if (i == 0) {
        int m = 0;
        sleep(5);
        printf("This is process: %d. The parent process is %d. The number m is %d.\n", (int)getpid(), (int)getppid(), m);
    } else {
        printf("This is not process i. The process ID: %d. The parent process ID: %d.\n", (int)getpid(), (int)getppid());
    }

    if (j == 0) {
        int m = 1;
        printf("This is process: %d. The parent process is %d. The number m is %d.\n", (int)getpid(), (int)getppid(), m);
    } else {
        printf("This is not process j. The process ID: %d. The parent process ID: %d.\n", (int)getpid(), (int)getppid());
    }
    
    wait(NULL);
    wait(NULL);
    return 0;
}
