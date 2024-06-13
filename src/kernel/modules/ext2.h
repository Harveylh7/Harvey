#ifndef __EXT2__
#define __EXT2__

#include <runtime/types.h>
#include <core/file.h>
#include <io.h>

/*
 *	Ext2 specification
 *  EXT2 文件系统超级块的定义
 */
struct ext2_super_block
{
    u32 s_inodes_count;      /* 总 inode 数量 */
    u32 s_blocks_count;      /* 总块数 */
    u32 s_r_blocks_count;    /* 保留给超级用户的块数 */
    u32 s_free_blocks_count; /* 空闲块数 */
    u32 s_free_inodes_count; /* 空闲 inode 数量 */
    u32 s_first_data_block;  /* 包含超级块结构的块号 */
    u32 s_log_block_size;    /* 用于计算块大小 = 1024 << s_log_block_size */
    u32 s_log_frag_size;     /* 用于计算碎片大小 */
    u32 s_blocks_per_group;  /* 每组块数 */
    u32 s_frags_per_group;   /* 每组碎片数 */
    u32 s_inodes_per_group;  /* 每组 inode 数量 */
    u32 s_mtime;             /* 文件系统上次挂载时间 */
    u32 s_wtime;             /* 文件系统上次写入时间 */
    u16 s_mnt_count;         /* 上次完全校验后的挂载次数 */
    u16 s_max_mnt_count;     /* 最大挂载次数 */
    u16 s_magic;             /* 文件系统魔数，= 0xEF53 */
    u16 s_state;             /* 文件系统状态 */
    u16 s_errors;            /* 检测到错误时的行为 */
    u16 s_minor_rev_level;   /* 次版本号 */
    u32 s_lastcheck;         /* 上次校验时间 */
    u32 s_checkinterval;     /* 最大校验间隔时间 */
    u32 s_creator_os;        /* 创建文件系统的操作系统，= 5 */
    u32 s_rev_level;         /* 版本级别，= 1 */
    u16 s_def_resuid;        /* 保留块的默认用户 ID */
    u16 s_def_resgid;        /* 保留块的默认组 ID */
    u32 s_first_ino;         /* 标准文件的第一个 inode */
    u16 s_inode_size;        /* inode 大小 */
    u16 s_block_group_nr;    /* 存储超级块结构的块组号 */
    u32 s_feature_compat;    /* 兼容特性 */
    u32 s_feature_incompat;  /* 不兼容特性 */
    u32 s_feature_ro_compat; /* 只读兼容特性 */
    u8 s_uuid[16];           /* 卷 UUID */
    char s_volume_name[16];  /* 卷名 */
    char s_last_mounted[64]; /* 文件系统上次挂载路径 */
    u32 s_algo_bitmap;       /* 压缩算法位图 */
    u8 s_padding[820];       /* 填充 */
} __attribute__((packed));

/*
 *	组描述符
 */
struct ext2_group_desc
{
    u32 bg_block_bitmap;      /* 块位图的第一个块号 */
    u32 bg_inode_bitmap;      /* inode 位图的第一个块号 */
    u32 bg_inode_table;       /* inode 表的第一个块号 */
    u16 bg_free_blocks_count; /* 空闲块数 */
    u16 bg_free_inodes_count; /* 空闲 inode 数量 */
    u16 bg_used_dirs_count;   /* 分配给目录的 inode 数量 */
    u16 bg_pad;               /* 填充，使结构体在 32 位边界对齐 */
    u32 bg_reserved[3];       /* 预留，用于未来实现 */
} __attribute__((packed));

/*
 *	inode 结构体
 */
struct ext2_inode
{
    u16 i_mode;        /* 文件类型和访问权限 */
    u16 i_uid;         /* 文件所有者的用户 ID */
    u32 i_size;        /* 文件大小 */
    u32 i_atime;       /* 最后访问时间 */
    u32 i_ctime;       /* 创建时间 */
    u32 i_mtime;       /* 最后修改时间 */
    u32 i_dtime;       /* 删除时间 */
    u16 i_gid;         /* 文件所有者的组 ID */
    u16 i_links_count; /* 链接计数 */
    u32 i_blocks;      /* 文件使用的块数（单位：512 字节块） */
    u32 i_flags;       /* 文件标志 */
    u32 i_osd1;        /* OS 特定的第一个字段 */

    /*
     * 块指针：
     * [0] -> [11] : 直接块指针（每块 32 位）
     * [12]        : 间接块指针
     * [13]        : 双重间接块指针
     * [14]        : 三重间接块指针
     */
    u32 i_block[15];

    u32 i_generation; /* 文件生成号 */
    u32 i_file_acl;   /* 文件访问控制列表 */
    u32 i_dir_acl;    /* 目录访问控制列表 */
    u32 i_faddr;      /* 文件碎片地址 */
    u8 i_osd2[12];    /* OS 特定的第二个字段 */
} __attribute__((packed));

/*
 * 目录项结构体
 */
struct ext2_directory_entry
{
    u32 inode;    /* inode 号，0 表示未使用 */
    u16 rec_len;  /* 到下一个目录项的偏移 */
    u8 name_len;  /* 文件名长度 */
    u8 file_type; /* 文件类型 */
    char name;    /* 文件名的第一个字符，实际文件名长度由 name_len 指定 */
} __attribute__((packed));

/*
 * EXT2 磁盘信息
 */
struct ext2_disk
{
    ext2_super_block *sb; /* 超级块指针 */
    ext2_group_desc *gd;  /* 组描述符指针 */
    u32 blocksize;        /* 块大小 */
    u16 groups;           /* 块组总数 */
    File *dev;            /* 设备文件指针 */
};

/* super_block: s_errors */
#define EXT2_ERRORS_CONTINUE 1 /* 错误时继续 */
#define EXT2_ERRORS_RO 2       /* 错误时只读 */
#define EXT2_ERRORS_PANIC 3    /* 错误时恐慌 */
#define EXT2_ERRORS_DEFAULT 1  /* 默认错误处理 */

/* inode: i_mode */
#define EXT2_S_IFMT 0xF000   /* 格式掩码 */
#define EXT2_S_IFSOCK 0xC000 /* 套接字 */
#define EXT2_S_IFLNK 0xA000  /* 符号链接 */
#define EXT2_S_IFREG 0x8000  /* 普通文件 */
#define EXT2_S_IFBLK 0x6000  /* 块设备 */
#define EXT2_S_IFDIR 0x4000  /* 目录 */
#define EXT2_S_IFCHR 0x2000  /* 字符设备 */
#define EXT2_S_IFIFO 0x1000  /* FIFO */

#define EXT2_S_ISUID 0x0800 /* SUID 位 */
#define EXT2_S_ISGID 0x0400 /* SGID 位 */
#define EXT2_S_ISVTX 0x0200 /* 粘滞位 */
#define EXT2_S_IRWXU 0x01C0 /* 用户访问权限掩码 */
#define EXT2_S_IRUSR 0x0100 /* 读取权限 */
#define EXT2_S_IWUSR 0x0080 /* 写入权限 */
#define EXT2_S_IXUSR 0x0040 /* 执行权限 */
#define EXT2_S_IRWXG 0x0038 /* 组访问权限掩码 */
#define EXT2_S_IRGRP 0x0020 /* 组读取权限 */
#define EXT2_S_IWGRP 0x0010 /* 组写入权限 */
#define EXT2_S_IXGRP 0x0008 /* 组执行权限 */
#define EXT2_S_IRWXO 0x0007 /* 其他人访问权限掩码 */
#define EXT2_S_IROTH 0x0004 /* 其他人读取权限 */
#define EXT2_S_IWOTH 0x0002 /* 其他人写入权限 */
#define EXT2_S_IXOTH 0x0001 /* 其他人执行权限 */

#define EXT2_INUM_ROOT 2 /* 根目录 inode 号 */

/*
 *	驱动类
 */
class Ext2 : public File
{
public:
    Ext2(char *n); /* 构造函数，接收文件系统名称 */
    ~Ext2();       /* 析构函数 */

    u32 open(u32 flag);                        /* 打开文件 */
    u32 close();                               /* 关闭文件 */
    u32 read(u32 pos, u8 *buffer, u32 sizee);  /* 从文件读取数据 */
    u32 write(u32 pos, u8 *buffer, u32 sizee); /* 向文件写入数据 */
    u32 ioctl(u32 id, u8 *buffer);             /* 控制设备 */
    u32 remove();                              /* 删除文件 */
    void scan();                               /* 扫描文件系统 */

    char *map;       /* 文件系统映射 */
    ext2_disk *disk; /* 磁盘信息 */
    int ext2inode;   /* ext2 inode */
private:
};

int ext2_check_disk(File *dev);                                     /* 检查磁盘 */
void ext2_get_disk_info(File *dev, Ext2 *fp);                       /* 获取磁盘信息 */
int ext2_read_gd(File *fdev, ext2_group_desc *gd, ext2_disk *info); /* 读取组描述符 */
int ext2_read_sb(File *dev, ext2_super_block *sb);                  /* 读取超级块 */
ext2_inode *ext2_read_inode(ext2_disk *hd, int i_num);              /* 读取 inode */
int ext2_is_directory(Ext2 *fp);                                    /* 判断是否为目录 */
int ext2_scan(Ext2 *dir);                                           /* 扫描目录 */
char *ext2_read_file(ext2_disk *hd, ext2_inode *inode);             /* 读取文件 */

#endif
