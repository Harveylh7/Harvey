#ifndef APROC_H
#define APROC_H

#include <runtime/types.h>
#include <runtime/list.h>
extern "C"{
#define KERNELMODE   0
#define USERMODE     1

struct process_st{
    int pid;
    process_st* parent;

    //寄存器状态
    struct {
        u32 eax,ecx,edx,ebx;
        u32 esp,ebp,esi,edi;
        u32 eip,eflags;
        u32 cs:16,ss:16,ds:16,es:16,fs:16,gs:16;
        u32 cr3;
    }regs __attribute__((packed));

    //内核栈信息
    struct {
        u32 esp0;
        u16 ss0;
    }kstack __attribute__((packed));

    //页目录指针
    struct page_directory *pd;

    //页面链表
    list_head pglist;


    //内存段地址
    char *b_exec;
    char *e_exec;
    char *b_bss;
    char *e_bss;
	char *b_heap;
	char *e_heap;

    //信号处理
    u32 signal;//32位信号
    void* sigfn[32];//信号的处理方法

    //进程虚拟地址
    void* vinfo;


}__attribute__((packed));





}




#endif