#include <stdbool.h>

struct inode {
    // inode编号
    int id;
    // 文件名
    char name[64];
    // 文件类型
    bool type[7];
    // 文件大小(使用blocks的个数)
    double blocks;
    // 文件上次inode变动时间
    int ctime;
    // 文件上次内容变动时间
    int mtime;
    // 文件上次访问时间
    int atime;
    // 文件删除时间
    int dtime;
    // 文件所属用户(UID)
    int owner;
    // 文件所属组(GID)
    int group;
    // 所属用户权限
    bool owner_p[3];
    // 组权限
    bool group_p[3];
    // 其他用户权限
    bool others_p[3];
    // inode links num
    int link_count;
    // blocks直接索引
    int index[12];
    // blocks间接索引
    int index1;
    // blocks二级索引
    int index2;
    // blocks三级索引
    int index3;
};

struct superblock {
    // inode num
    int inodes;
    // free inode num
    int free_inodes;
    // blocks num
    double blocks;
    // free blocks num
    double free_blocks;
};

struct blockgroup {
    // blocks bitmap
    double blocks_bitmap;
    // inode bitmap
    double inode_bitmap;
    // inodes table bitmap
    double inode_table_bitmap;
    // free blocks count
    int free_blocks_count;
    // free inodes count
    int free_inodes_count;
    // block groups flags
    // 1. node table and bitmap are not initialized
    // 2. block bitmap is not initialized
    // 3. inode table is zeroed
    bool flags[3];
};