//
// Created by yellowsea on 17-10-9.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>

#define SERVPORT 2378
#define BACKLOG 10
#define MAX_FILENAME_LEN 256
#define CHIP 6

struct File_info {
    char name[MAX_FILENAME_LEN];
    unsigned long size;
    unsigned int chips;
};


// 获取文件大小
unsigned long get_file_size(const char *path) {
    struct stat statbuff;
    if (stat (path, &statbuff) < 0) {
        return 0;
    }
    return (unsigned long)statbuff.st_size;
}


int main()
{
    int sockfd, client_fd;
    struct sockaddr_in my_addr;
    struct sockaddr_in remote_addr;
    int sin_size;
    char filename[MAX_FILENAME_LEN];


    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket.....");
        exit(1);
    }

    my_addr.sin_family=AF_INET;
    my_addr.sin_port=htons(SERVPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero),8);

    if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))== -1) {
        perror("bind...");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1){
        perror("listen...");
        exit(1);
    }

    while(1){
        sin_size = sizeof(struct sockaddr_in);
        if ((client_fd = accept(sockfd, (struct sockaddr *)&remote_addr,&sin_size)) == -1) {
            perror("accept..");
            continue;
        }
        printf("received a connection from %s\n", inet_ntoa(remote_addr.sin_addr));

        if (!fork()) {
            // get filename from client
            int recvbytes;
            if ((recvbytes = recv(client_fd, filename, MAX_FILENAME_LEN, 0)) == -1) {
                perror("recv erorr!");
            }

            filename[recvbytes] ='\0';
            printf("A connection from %s request for file: %s\n", inet_ntoa(remote_addr.sin_addr), filename);

            // get file length by filename
            struct File_info file;
            strcpy(file.name, filename);
            file.size = get_file_size(filename);
            file.chips = CHIP;

            // send to client
            if (send(client_fd, &file, sizeof(file), 0) == -1){
                perror("send...");
                exit(0);
                close(client_fd);
            }

            close(client_fd);
        }
    }

}