#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>

#define MAXDATASIZE 1024
#define MAXFILENAMESIZE 512
#define SERVPORT 5555
#define BACKLOG 10

typedef struct {
    char name[MAXFILENAMESIZE];
    int length;
    int seek;
    pthread_t threadId;
}FILEINFO;

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *hent;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons((!argv[1]) ? SERVPORT : atoi(argv[1]));
    bzero(&(server_addr.sin_zero), 8);

    // Bind the socket to an address and a port
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0) {
        perror("Bind");
        exit(EXIT_FAILURE);
    }
    hent = gethostent();
    for (int i = 0; hent->h_addr_list[i]; i++) {
        printf("Socket bind to %s:%d\n",  inet_ntoa(*(struct in_addr*)(hent->h_addr_list[i])), ntohs(server_addr.sin_port));
    }

    // Start a listener
    if (listen(sockfd, BACKLOG) < 0) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }
    printf("==================== File Transfer Server ====================\n");

    socklen_t clientlen = sizeof(struct sockaddr_in);
    char dir[MAXDATASIZE];
    system("tree -h ../src/ > dir.txt");
    FILE *fp = fopen("dir.txt", "r");
    fread(dir, 1, MAXDATASIZE, fp);

    // Communication with the connection with the client
    while(1) {
        struct sockaddr_in client_addr;
        int client_fd;

        if ((client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &clientlen)) < 0) {
            perror("Accept");
            continue;
        }
        printf("Client: %s is connecting...\n", inet_ntoa(client_addr.sin_addr));

        // Send the source dir to the client
        if (send(client_fd, dir, MAXDATASIZE, 0) < 0) {
            perror("Send Dir");
            continue;
        }

        // Recieve the name of the file which the client need
        char buffer[MAXDATASIZE] = {0};
        int length = recv(client_fd, buffer, MAXDATASIZE, 0);
        if (length < 0) {
            perror("Recv filename");
            continue;
        }

        char filename[MAXFILENAMESIZE+1] = {0};
        char map[MAXFILENAMESIZE+7] = "../src/";
        strncpy(filename, buffer, strlen(buffer));
        printf("%s\n", filename);
        strcat(map, filename);
        FILE *fp = fopen(map, "rb");
        if (fp == NULL) {
            printf("Cannot open file: %s\n", buffer);
            continue;
        }
        int nCount = 0, i = 0;
        while ((nCount = fread(buffer, 1, MAXDATASIZE, fp)) > 0) {
            printf("%d: file_block_length = %d\n", i++, nCount);
            send(client_fd, buffer, MAXDATASIZE, 0);
            bzero(buffer, sizeof(buffer));
        }
        shutdown(client_fd, SHUT_WR);
        if (recv(client_fd, buffer, MAXDATASIZE, 0) < 0) {
            perror("Recv result");
            continue;
        }
        printf("%s\n", buffer);
        fclose(fp);
        close(client_fd);
    }

    // Close the server and the socket
    close(sockfd);
    return 0;
}
