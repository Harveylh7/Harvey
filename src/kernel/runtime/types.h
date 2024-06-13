#ifndef TYPES_H
#define TYPES_H

//定义C语言类型
typedef unsigned char u8;//无符号8位的数据
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;



typedef signed char  s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;



typedef unsigned char u_char;

typedef unsigned int size_t;
typedef int pid_t;
typedef s64 ino_t;//索引节点号
typedef s64 off_t;//文件偏移量
typedef int dev_t;//设备号
typedef int mode_t;//文件模式（权限）
typedef int nlink_t;//硬链接计数
typedef int uid_t;//用户ID
typedef int gid_t;//组ID
typedef int blksize_t;//块大小
typedef s64 blkcnt_t;//块计数
#define time_t s64




struct stat_fs{//描述文件状态信息
    dev_t st_dev;
    ino_t st_ino;
    mode_t st_mode;
    nlink_t st_nlink;
    uid_t st_uid;
    gid_t st_gid;
    dev_t st_rdev;
    off_t st_size;
    blksize_t st_blksize;
    blkcnt_t st_blocks;
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
    
};


//返回码
enum{
    RETURN_OK = 0,
    NOT_DEFINED=-1,
    ERROR_MEMORY=-2,
    PARAM_NULL=-3,//空指针
    ERROR_PARAM=-4,
    RETURN_FAILURE = -128

};

typedef void (*int_handler)(void);

#define NULL 0
#define true 1
#define false 0




#endif