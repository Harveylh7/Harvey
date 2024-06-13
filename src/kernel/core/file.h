#ifndef FILE_H
#define FILE_H

#include <runtime/types.h>

enum
{ 
    TYPE_FILE,
    TYPE_DIRECTORY,
    TYPE_DEVICE,
    TYPE_PROCESS,
    TYPE_LINK
};


class File
{
public:
    File(char *n, u8 t);
    ~File();

    virtual u32 open(u32 flag);
    virtual u32 close();
    virtual u32 read(u32 pos, u8 *buffer, u32 size);
    virtual u32 write(u32 pos, u8 *buffer, u32 size);
    virtual u32 ioctl(u32 id, u8 *buffer);
    virtual u32 remove();
    virtual void scan();

    void checkName();

    u32 addChild(File *n);            // 添加一个字目录，返回值可能是状态码
    File *createChild(char *n, u8 t); // 创建一个子目录或者子文件
    File *find(char *n);
    u32 mmap(u32 sizee, u32 flags, u32 offset, u32 prot); // 文件或其他资源映射到进程的地址空间中，prot可能是内存保护选项（权限）

    // 文件设置属性的接口
    void setSize(u32 t);
    void setType(u8 t);
    void setParent(File *n); // 父目录
    void setChild(File *n);  // 子目录
    void setNext(File *n);
    void setPrec(File *n);
    void setLink(File *n);
    void setName(char *n);

    char *getName();
    File *getParent();
    File *getChild();
    File *getNext();
    File *getPrec();
    File *getLink();
    u8 getType();
    u32 getSize();
    u32 getInode();

    stat_fs stat();

protected:
    static u32 inode_system;

    char *map_memory; // 映射文件到内存

    char *name;
    u32 size;
    u8 type; // 普通文件、目录或其他类型。
    u32 inode;
    File *dev;  // 指向表示主设备文件的文件对象
    File *link; // 指向实际文件的指针，如果当前文件是一个链接文件的话。在操作系统中，链接文件指向另一个文件

    File *master; // 指向当前文件对象所属的主进程的文件对象的指针

    File *parent;
    File *child;
    File *next;
    File *prec;

    File *device; //文件对象所关联的设备的主文件或设备文件对象

};
#endif