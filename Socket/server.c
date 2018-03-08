#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>

#define SERVPORT 5555
#define BACKLOG 10
#define MAXDATASIZE 1024

void quit_transmission(int sig) {
    printf("recieve a quit signal = %d\n", sig);
    exit(EXIT_SUCCESS);
}


int main() {
    int sockfd, client_fd;
    struct sockaddr_in server;
    struct sockaddr_in client;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error!");
        exit(EXIT_FAILURE);
    }
    printf("Create Socket success!\n");
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = SERVPORT;
    bzero(&(server.sin_zero), 8);

    // setsockopt开启非阻塞模式，确保服务器不用等待TIME_WAIT状态结束就可以重启服务器，继续使用原来的端口号
    int on = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("Setcokopt");
        exit(EXIT_FAILURE);
    }
    if (bind(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) < 0) {
        perror("Bind");
        exit(EXIT_FAILURE);
    }
    printf("Socket bind to %s:%d\n", inet_ntoa(server.sin_addr), SERVPORT);

    if (listen(sockfd, BACKLOG) < 0) {
        perror("listen error!");
        exit(EXIT_FAILURE);
    }

    printf("==================== Server Socket Waiting for Client Connection ====================\n");

    socklen_t clientlen = sizeof(struct sockaddr_in);
    if ((client_fd = accept(sockfd, (struct sockaddr *)&client, &clientlen)) < 0) {
        perror("Accept");
        exit(EXIT_FAILURE);
    }
    printf("Client: %s is connect...\n", inet_ntoa(client.sin_addr));

    pid_t pid;
    pid = fork();
    if (pid < 0) {
        perror("Fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        signal(SIGUSR1, quit_transmission);
        char send_buf[MAXDATASIZE] = {0};
        while (fgets(send_buf, MAXDATASIZE, stdin) != NULL) {
            write(client_fd, send_buf, MAXDATASIZE);
            bzero(send_buf, MAXDATASIZE);
        }
        exit(EXIT_SUCCESS);
    } else {
        char recv_buf[MAXDATASIZE] = {0};
        while(1) {
            bzero(recv_buf, MAXDATASIZE);
            int ret = read(client_fd, recv_buf, MAXDATASIZE);
            if (ret < 0) {
                perror("Read");
                exit(EXIT_FAILURE);
            } else if (ret == 0) {
                printf("client is close!\n");
                break;
            }
            printf("Client: ");
            fputs(recv_buf, stdout);
        }
        kill(pid, SIGUSR1);
    }
    close(client_fd);
    close(sockfd);
    return 0;
}
