#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#define SERVPORT 2379
#define BACKLOG 10
#define MAX_FILENAME_LEN 256
#define BUFSIZE 1024

struct file_chip {
    char filename[MAX_FILENAME_LEN];
    unsigned long offset;
    unsigned long size;
};

int main()
{
    int sockfd,client_fd;
    struct sockaddr_in my_addr;
    struct sockaddr_in remote_addr;
    int sin_size;

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
            // get file chip info
            char buf[sizeof(struct file_chip)];
            struct file_chip chip;
            int recvbytes;

            memset(buf, 0, sizeof(buf));
            if ((recvbytes = recv(client_fd, buf, sizeof(buf), 0)) ==-1) {
                perror("recv erorr!");
            }

            memcpy(&chip, buf, sizeof(buf));

            printf("Filename: %s\n", chip.filename);
            printf("Offset: %ld\n", chip.offset);
            printf("Size: %ld\n", chip.size);

            // transfer file
            FILE *fp;
            if((fp = fopen(chip.filename, "r")) == NULL) {
                fprintf(stderr, "can't open the FILE: %s\n", chip.filename);
                close(client_fd);
                exit(1);
            }

            // 定位文件指针位置
            fseek(fp, chip.offset, SEEK_SET);

            unsigned long left = chip.size;
            size_t byte;
            char buffer[BUFSIZE];

            while(byte = fread(buffer, sizeof(char), left > BUFSIZE ? BUFSIZE : left, fp)) {
                if(send(client_fd, buffer, byte, 0) < 0) {
                    fprintf(stderr, "File send Failed: %s\n", chip.filename);
                }
                //        bzero(buffer, BUFSIZE);
                left -= byte;
                if(left <= 0)   break;
            }
            close(client_fd);
        }
    }

}