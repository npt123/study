#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXFILENAMESIZE 512
#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;

    if (argc < 3) {
        fprintf(stderr, "usage prog <hostname> <port>\n");
        exit(EXIT_FAILURE);
    }
    // Prepare the socket for connection
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }
    int servport = atoi(argv[2]);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(servport);
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    bzero(&(server_addr.sin_zero), 8);

    char buffer[BUF_SIZE] = {0};

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0) {
        perror("Connect");
        exit(EXIT_FAILURE);
    }
    printf("Connect to the server: %s:%d\n", argv[1], servport);
    printf("This is the dir of the sources in the server: \n");
    if (recv(sockfd, buffer, BUF_SIZE, 0) < 0) {
        perror("Recv dir");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", buffer);
    printf("Please input the filename you need:\n");

    char filename[MAXFILENAMESIZE]={0};
    while (fgets(filename, MAXFILENAMESIZE, stdin) == NULL || strcmp(filename, "\n") == 0) {
        printf("Please input the right filename!\n");
    }
    filename[strlen(filename) - 1] = 0;
    if (send(sockfd, filename, MAXFILENAMESIZE, 0) < 0) {
        perror("Send");
        exit(EXIT_FAILURE);
    }
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int nCount;
    while((nCount = recv(sockfd, buffer, BUF_SIZE, 0)) > 0) {
        fwrite(buffer, nCount, 1, fp);
    }
    char message[] = "File transfer success!\n";
    printf("%s", message);
    if (send(sockfd, message, strlen(message), 0) < 0) {
        perror("Send");
        exit(EXIT_FAILURE);
    }

    fclose(fp);
    close(sockfd);
    return 0;
}
