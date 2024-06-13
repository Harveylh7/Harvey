#ifndef FILESYSTEM_H
#define FILESYSTEM_H


#include <runtime/types.h>
#include <core/file.h>

class Filesystem
{
public:
    Filesystem();
    ~Filesystem();


    void init();

    void mknod(char* module,char* name,u32 flag);//创建文件节点（设备文件、普通文件等）

    File* path(char* p);//根据路径获取文件对象
    File* path_parent(char* p,char *fname);//获取给定路径的父目录并返回子文件名

    
    u32 link(char* fname,char *newf);//创建硬链接。
    
    u32 addFile(char* dir,File* fp);//将文件添加到目录中。

    File* pivot_root(File* targetdir);//更改根文件系统。

    File* getRoot();//获取当前的根文件系统。


private:
    File* root;//所有其他文件和目录。
    File* dev;//包含与设备相关的特殊文件，如硬盘、终端设备等。
    File* var;//表示可变数据文件目录（"/var"）。该目录通常包含系统运行时产生的数据文件，如日志、锁文件、临时文件等。
};


extern Filesystem		fsm;

#endif