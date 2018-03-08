#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "inode.c"

void init() {
    // 设备A初始化
    for (int i = 0; i < 10000; ++i) {
        int index[1000] = {0};
        char block_name[15] = "./sda/";
        char num[6];
        sprintf(num, "%d", i+1);
        strcat(block_name, num);
        FILE *block_file = fopen(block_name, "wb");
        fwrite(index, sizeof(index), 1, block_file);
        fclose(block_file);
    }
    // 设备B初始化
    for (int i = 0; i < 10000; ++i) {
        int index[1000] = {0};
        char block_name[15] = "./sdb/";
        char num[6];
        sprintf(num, "%d", i+1);
        strcat(block_name, num);
        FILE *block_file = fopen(block_name, "wb");
        fwrite(index, sizeof(index), 1, block_file);
        fclose(block_file);
    }
}

int main() {
    init();
    return 0;
}