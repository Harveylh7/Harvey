#ifndef VMM_H
#define VMM_H

#include <runtime/types.h>
#include <runtime/list.h>
#include <runtime/alloc.h>
#include <x86.h>

// 虚拟内存管理
extern "C"
{

    struct page
    {
        char *v_addr;   // 虚拟地址
        char *p_addr;   // 物理地址
        list_head list; // 链表节点，用于管理页表中的多个页
    };

    // 页目录结构体
    struct page_directory
    {
        page *base;   // 页表的基地址
        list_head pt; // 页表链表
    };

    struct vm_area
    {
        char *vm_start; // 虚拟内存起始地址
        char *vm_end;   // 虚拟内存结束地址（不包含）
        list_head list; // 链表节点，用于管理多个虚拟内存区域
    };

    // 内存定义，表示进程的内存
    typedef page_directory proc_memory;

    // 外部变量，指向内核堆顶端
    extern char *kern_heap;

    // 外部变量，指向内核空闲页链表的起始
    extern list_head kern_free_vm;

    // 内核页目录
    extern u32 *pd0;

    // 内存位图
    extern u8 mem_bitmap[];

    // 已使用的内核内存数量
    extern u32 kmalloc_used;

// 宏定义，将一个页标记为已使用或空闲 ？
#define set_page_frame_used(page) mem_bitmap[((u32)page) / 8] |= (1 << (((u32)page) % 8))//余数找到具体的下标，取模获得具体的比特位
#define release_page_frame(p_addr) mem_bitmap[((u32)p_addr / PAGESIZE) / 8] &= ~(1 << (((u32)p_addr / PAGESIZE) % 8))//通过物理地址找到对应的页号并移除,p_addr / PAGESIZE为第几个页

    // 从内存选择一个空闲页
    char *get_page_frame(void);

    // 从堆中选择一个空闲页并关联到虚拟地址
    struct page *get_page_from_heap(void);
    int release_page_from_heap(char *);
    // 初始化内存管理结构
    void Memory_init(u32 high_mem);

    // 这个函数的作用是创建一个新的页目录。
    struct page_directory *pd_create(void);
    // 销毁一个已有的页目录
    int pd_destroy(struct page_directory *);
    // 复制一个已有的页目录
    struct page_directory *pd_copy(struct page_directory *pdfather);

    // 向内核的页目录添加一个页表条目。
    int pd0_add_page(char *, char *, int);
    // 向当前页目录添加一个页表条目。
    int pd_add_page(char *, char *, int, struct page_directory *);
    // 从当前页目录中删除一个页表条目。
    int pd_remove_page(char *);
    // 获取与给定虚拟地址关联的物理地址。
    char *get_p_addr(char *);

#define KMALLOC_MINSIZE 16

    // 内核内存分配的头信息结构
    struct kmalloc_header
    {
        //4字节
        unsigned long size : 31; // 存储了该内存块的大小信息。
        unsigned long used : 1;  // 指示该内存块当前是否被使用。
    } __attribute__((packed));

    class Vmm
    {
    public:
        void init(u32 high);                         // 初始化虚拟内存管理器，通常在系统启动时调用。
        proc_memory *createPM();                     // 为一个新进程创建页目录。
        void switchPM(proc_memory *ad);              // 切换当前进程的页目录。
        void map(proc_memory *ad, u32 phy, u32 adr); // 将一个物理内存页映射到指定的虚拟地址空间中。
        void kmap(u32 phy, u32 virt);                // 将物理内存页映射到内核的虚拟地址空间中。
    };
}

extern Vmm vmm;
#endif