#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>

#define MAXDATASIZE 1024

void quit_transmission(int sig) {
    printf("recieve a quit signal = %d\n", sig);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server;

    if (argc < 3) {
        fprintf(stderr, "Please enter the server's hostname and port!\n");
        exit(EXIT_FAILURE);
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }
    int servport = atoi(argv[2]);
    server.sin_family = AF_INET;
    server.sin_port = servport;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    bzero(&(server.sin_zero), 8);
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr)) < 0) {
        perror("Connect");
        exit(EXIT_FAILURE);
    }
    printf("Connect to the server: %s:%d\n", argv[1], servport);

    pid_t pid;
    pid = fork();
    if (pid < 0) {
        perror("Fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        char recv_buf[MAXDATASIZE];
        while(1) {
            bzero(recv_buf, MAXDATASIZE);
            int ret = read(sockfd, recv_buf, sizeof(recv_buf));
            if (ret < 0) {
                perror("Read");
                exit(EXIT_FAILURE);
            } else if (ret == 0) {
                printf("server is close!\n");
                break;
            }
            printf("Server: ");
            fputs(recv_buf, stdout);
        }
        close(sockfd);
        kill(getppid(), SIGUSR1);
        exit(EXIT_SUCCESS);
    } else {
        signal(SIGUSR1, quit_transmission);
        char send_buf[MAXDATASIZE];
        while (fgets(send_buf, MAXDATASIZE, stdin) != NULL) {
            int set = write(sockfd, send_buf, MAXDATASIZE);
            if (set < 0) {
                perror("Write");
                exit(EXIT_FAILURE);
            }
            bzero(send_buf, MAXDATASIZE);
        }
        close(sockfd);
    }

    return 0;
}
