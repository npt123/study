#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define SERVPORT 5555
#define BACKLOG 10
#define BUF_SIZE 1024
#define MAXFILENAMESIZE 512

int main() {
    int sockfd, client_fd, i = 1;
    struct sockaddr_in server_addr;
    struct hostent *hent;
    struct sockaddr_in client_addr;

    // Create a socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        exit(EXIT_FAILURE);
    }
    printf("Create socket success!\n");
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(SERVPORT);
    bzero(&(server_addr.sin_zero), 8);

    // Bind the socket to a address and a port
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0) {
        perror("Bind");
        exit(EXIT_FAILURE);
    }
    hent = gethostent();
    printf("Socket bind to %s:%d\n", inet_ntoa(*(struct in_addr*)(hent->h_addr_list[0])), ntohs(server_addr.sin_port));

    // Start a listen
    if (listen(sockfd, BACKLOG) < 0) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }
    printf("==================== Server Socket Waiting For Client Connection ====================\n");

    socklen_t clientlen = sizeof(struct sockaddr_in);
    char dir[BUF_SIZE];
    system("tree -h src/ > dir.txt");
    FILE *fp = fopen("dir.txt", "r");
    fread(dir, 1, BUF_SIZE, fp);

    // Communication with the connection with the client
    while(1) {
        if ((client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &clientlen)) < 0) {
            perror("Accept");
            continue;
        }
        printf("Client: %s is connecting...\n", inet_ntoa(client_addr.sin_addr));

        // Send the source dir to the client
        if (send(client_fd, dir, BUF_SIZE, 0) < 0) {
            perror("Send Dir");
            continue;
        }

        // Recieve the name of the file which the client need
        char buffer[BUF_SIZE] = {0};
        int length = recv(client_fd, buffer, BUF_SIZE, 0);
        if (length < 0) {
            perror("Recv filename");
            continue;
        }

        char filename[MAXFILENAMESIZE+1] = {0};
        char map[MAXFILENAMESIZE+7] = "./src/";
        strncpy(filename, buffer, strlen(buffer));
        printf("Client request: %s\n", filename);
        strcat(map, filename);
        FILE *fp = fopen(map, "rb");
        if (fp == NULL) {
            printf("Cannot open file: %s", buffer);
            continue;
        }

        fseek(fp, 0L, SEEK_END);
        int fileSize = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        // Send File Size
        if (send(client_fd, &fileSize, sizeof(int), 0) < 0) {
            perror("Send file size");
            continue;
        }
        int nCount = 0;
        while ((nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
            printf("%d: file_block_length = %d\n", i++, nCount);
            send(client_fd, buffer, BUF_SIZE, 0);
            
            bzero(buffer, sizeof(buffer));
        }
        shutdown(client_fd, SHUT_WR);
        if (recv(client_fd, buffer, BUF_SIZE, 0) < 0) {
            perror("Recv result");
            continue;
        }
        printf("%s\n", buffer);
        fclose(fp);
    }
    printf("File Transfer Client closed!\n");
    close(client_fd);
    close(sockfd);
    return 0;
}
