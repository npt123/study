#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

struct PCB {
    char name[16];
    int priority;
    int RunTime;
    int OccupiedTime;
    int status;
};

void FCFS() {
    printf("FCFS");
    return;
}
void SJF() {
    printf("SJF");
    return;
}
void HRRN() {
    printf("HRRN");
    return;
}

int main(int argc, void *argv[]) {
    int algorithm = 0;
    while(algorithm == 0) {
        printf("Please choose the algorithm(input the number):\n");
        printf("  [1]FCFS\n");
        printf("  [2]SJF\n");
        printf("  [3]HRRN\n");
        scanf("%d", algorithm);
    }
    switch (algorithm) {
        case 1:
            FCFS();
            break;
        case 2:
            SJF();
            break;
        case 3:
            HRRN();
            break;
    }
    return 0;
}
