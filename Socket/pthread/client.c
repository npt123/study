#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define FILEINFO_SERVPORT 2378
#define FILETRAN_SERVPORT 2379
#define MAXDATASIZE 100
#define MAX_FILENAME_LEN 256
#define BUFSIZE 1024
#define MAX_THR_NUM 9
#define MAX_CHIP_NUM 9

// 文件的信息
struct File_info {
    char name[MAX_FILENAME_LEN];
    unsigned long size;
    unsigned int chips;
};

// 文件分块的结构
struct file_chip {
    char filename[MAX_FILENAME_LEN];
    unsigned long offset;
    unsigned long size;
};

// 多线程传参
struct thr_params {
    struct hostent *host;
    struct file_chip *chip;
    int chip_no;
};



void get_filename(char *filename) {
    printf("Please input filename: \n");
    scanf("%s", filename);
}


// 合并并且删除临时文件
int file_union(char *filename, int chip_no) {
    char cmd_union[300];
    char cmd_rm[300];
    sprintf(cmd_union, "cat %s.part* > %s", filename, filename);
    sprintf(cmd_rm, "rm -rf %s.part*", filename);
    system(cmd_union);
    system(cmd_rm);
}



int thr_func(void *arg) {
    struct thr_params *params = (struct thr_params *)arg;
    return transmission(params->host, params->chip, params->chip_no);
}

// 获取文件大小，分片等信息
int prework(const struct hostent *host, struct File_info *file) {
    struct sockaddr_in serv_addr;
    int sockfd, recvbytes;
    char filename[MAX_FILENAME_LEN];

    get_filename(filename);

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket?");
        return (1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(FILEINFO_SERVPORT);
    serv_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(serv_addr.sin_zero),8);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1){
        perror("connect?");
        return (1);
    }


    if(send(sockfd, filename, strlen(filename), 0) == -1) {
        perror("File name send error!");
        return (1);
    }

    // recv file info

    char buf[sizeof(struct File_info)];
    memset(buf, 0, sizeof(buf));

    if ((recvbytes = recv(sockfd, buf, sizeof(buf), 0)) == -1) {
        perror("recv?");
        return (1);
    }

    memcpy(file, buf, sizeof(buf));

    close(sockfd);
    return 0;
}


int transmission(const struct hostent *host, struct file_chip *chip, int chip_no) {
    struct sockaddr_in serv_addr;
    int sockfd, recvbytes;


    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket?");
        return (1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(FILETRAN_SERVPORT);
    serv_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(serv_addr.sin_zero),8);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1){
        perror("connect?");
        return (1);
    }

    if(send(sockfd, chip, sizeof(struct file_chip), 0) == -1) {
        perror("File chip send error!");
        return (1);
    }


    FILE *fp;
    char filename[MAX_FILENAME_LEN];
    sprintf(filename, "%s.part%d", chip->filename, chip_no);
    if((fp = fopen(filename, "w")) == NULL) {
        fprintf(stderr, "File: %s create failed\n", filename);
    }

    // recv file
    char buffer[BUFSIZE];
    int byte;
    unsigned long left = chip->size;
    // 这里这样接收文件，会不会因为网络的问题造成包的顺序与发送的不一致？
    // 但是我在网上找了很久，都没提到这个问题，都是用这个方案来传输文件。
    // 同样用select的话也会有这个问题。
    while((byte = recv(sockfd, buffer, BUFSIZE, 0)) > 0) {
        if(fwrite(buffer, sizeof(char), byte, fp) < byte) {
            fprintf(stderr, "File write error!\n");
        }
        bzero(buffer, BUFSIZE);
        left -= byte;
        if(left <= 0)   break;
    }
    if(left > 0) {
        fprintf(stderr, "File is incomplete!\n");
        close(sockfd);
        fclose(fp);
        return 1;
    }
    close(sockfd);
    fclose(fp);
    return 0;
}




int main(int argc, char *argv[]){

    time_t startT, endT;
    double totalT;
    startT = time(NULL);
    struct hostent *host;
    struct File_info file;

    if (argc < 2){
        fprintf(stderr,"Please enter the server's hostname!\n");
        exit(1);
    }
    if ((host = gethostbyname(argv[1]))==NULL){
        herror("gethostbyname?");
        exit(1);
    }

    if (prework(host, &file) != 0) {
        perror("prework?");
    }

    if(file.size <= 0) {
        fprintf(stderr, "Can't find file in server!\n");
        exit(1);
    }

    // slice
    int i;
    unsigned long left = file.size;
    unsigned long offset = 0L;
    unsigned long chip_size = file.size / file.chips;
    pthread_t thr_pid[MAX_THR_NUM];
    struct file_chip chip[MAX_THR_NUM];
    struct thr_params params[MAX_CHIP_NUM];

    for(i = 1; i <= file.chips; i++) {

        strcpy(chip[i].filename, file.name);
        chip[i].offset = offset;
        if(i == file.chips)
            chip[i].size = file.size - offset;
        else
            chip[i].size = chip_size;

        params[i].host = host;
        params[i].chip = &chip[i];
        params[i].chip_no = i;

        if(pthread_create(&thr_pid[i-1], NULL, thr_func, (void *)&params[i]) != 0) {
            fprintf(stderr, "File chip No.%d, thread create failed!\n", i);
        }

        offset += chip_size;
    }

    int status;
    for(i = 0; i < file.chips; i++) {
        if(pthread_join(thr_pid[i], (void *)&status) != 0){
            fprintf(stderr, "thr_id %d join failed\n", i+1);;
        }
        if(status != 0) {
            fprintf(stderr, "File chip No.%d, recv failed\n", i+1);
        }
    }


    endT = time(NULL);
    totalT = difftime(endT, startT);
    printf("total: %f\n", totalT);

    // 文件合并
    file_union(file.name, file.chips);

    printf("main thread end\n");


    return 0;
}