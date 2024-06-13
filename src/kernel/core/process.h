#ifndef PROC_H
#define PROC_H


#include <core/file.h>
#include <runtime/list.h>
#include <archprocess.h>	/* definition de process_st */
#include <core/signal.h>
#include <runtime/buffer.h>
#include <api/dev/proc.h>
#include <config.h>

//proc.h中有定义
#define ZOMBIE	PROC_STATE_ZOMBIE //僵尸状态
#define CHILD	PROC_STATE_RUN    //运行状态

struct openfile
{
    u32 mode;//表示打开文件的模式或者权限
    u32 ptr;//表示文件当前的读写指针位置，即指示下一次读写操作的位置。
    File* fp;//指向表示打开的文件的 File 结构或对象。
};


class Process : public File{
public:
    Process(char* n);
    ~Process();

    //文件操作方法
    u32 open(u32 flag);
    u32 close();
    u32 read(u32 pos,u8* buffer,u32 size);
    u32 write(u32 pos,u8* buffer,u32 size);
    u32 ioctl(u32 id,u8*buffer);
    u32 remove();
    void scan();

    //进程控制的方法
    u32 create(char* file,int argc,char **argv);
    void sendSignal(int sig);//用于向进程发送信号
    u32 wait();//等待进程结束

    //用于管理进程打开的文件
    u32 addFile(File* fp,u32 m);
    File* getFile(u32 fd);
    void deleteFile(u32 fd);
    openfile* getFileInfo(u32 fd);

    //退出和创建进程
    void exit();
    int fork();

    //用于设置和获取进程的状态、进程ID、父进程等信息
    void setState(u8 st);
    u8 getState();
    void setFile(u32 fd,File* fp,u32 ptr,u32 mode);
    void setPid(u32 st);
    u32 getPid();

    //设置当前进程的下一个进程
    void setPNext(Process* p);

    //进行进程调度
    Process* schedule();
    //返回选择的下一个要执行的进程的指针
    Process* getPNext();
    //获取当前进程的父进程
    Process* getPParent();
    //获取当前信息的结构体
    process_st* getPInfo();
    //设置当前的父进程
    void setPParent(Process*p);

    //重置进程信息
    void reset_pinfo();


    //存储当前进程的详细信息，PID、状态等。
    process_st info;

    //获取当前进程的当前工作目录。
    File* getCurrentDir();

    //设置当前进程的当前工作目录。
    void setCurrentDir(File* f);

    u32 getPid()
    {
        return pid;
    }
protected:
    static u32 proc_pid;//所有类的实例共享相同的值。在这里，proc_pid 可能被用作为新创建的进程分配一个唯一的ID。

    u32 pid;
    u8 state;
    Process* pparent;//指向父进程
    Process* pnext;//指向下一个进程
    openfile openfp[CONFIG_MAX_FILE];//存储进程打开的文件的信息的数组
    proc_info ppinfo;//存储有关进程的详细信息
    File* cdir;//指向当前进程工作目录

    Buffer* ipc;//指向缓冲区的指针，用于进程间通信。在操作系统中，进程通常需要进行通信，以实现协作或数据交换等功能。

    static char* default_tty;//这是一个静态指针，指向默认的终端设备。在某些系统中，进程可能需要知道默认的终端设备以进行输入输出操作。
};


#endif