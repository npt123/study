#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define N 10
#define PSIZE 10

sem_t memw[N];
sem_t memr[N];

void pr(sem_t *sem) {
    int n = sem_wait(sem);
    if (n != 0) {
        perror('sem_wait');
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    for (int i = 0; i < N; ++i) {
        mem[i] = 1;
    }

    return 0;
}