#ifndef __X86__
#define __X86__

#include <runtime/types.h>

#define IDTSIZE		0xFF	/* 中断描述符表（IDT）的最大描述符数量 */
#define GDTSIZE		0xFF	/* 全局描述符表（GDT）的最大描述符数量 */

#define IDTBASE		0x00000000	/* IDT 应存放的物理地址 */
#define GDTBASE		0x00000800	/* GDT 应存放的物理地址 */

#define INTGATE  0x8E00		/* 用于处理中断的门描述符类型 */
#define TRAPGATE 0xEF00		/* 用于实现系统调用等陷阱的门描述符类型 */

#define	KERN_PDIR			0x00001000  /* 内核页目录的物理地址 */   //2^12
#define	KERN_STACK			0x0009FFF0  /* 内核栈的顶部地址 */
#define	KERN_BASE			0x00100000  /* 内核的起始地址 */
#define KERN_PG_HEAP		0x00800000  /* 内核页堆的起始地址 */
#define KERN_PG_HEAP_LIM	0x10000000  /* 内核页堆的结束地址 */
#define KERN_HEAP			0x10000000  /* 内核堆的起始地址 */
#define KERN_HEAP_LIM		0x40000000  /* 内核堆的结束地址 */      //2^30   1G

#define	USER_OFFSET 		0x40000000  /* 用户空间的起始地址 */
#define	USER_STACK 			0xE0000000  /* 用户栈的顶部地址 */

#define KERN_PG_1			0x400000  /* 内核的第一个页的起始地址 */ //2^22  4MB    
#define KERN_PG_1_LIM 		0x800000  /* 内核的第一个页的结束地址 */ //2^23	8MB	

#define	VADDR_PD_OFFSET(addr)	((addr) & 0xFFC00000) >> 22  /* 从虚拟地址中提取页目录的偏移量 */  //1111111111000000000000000000000
#define	VADDR_PT_OFFSET(addr)	((addr) & 0x003FF000) >> 12  /* 从虚拟地址中提取页表的偏移量 */    //00000000001111111111000000000000
#define	VADDR_PG_OFFSET(addr)	(addr) & 0x00000FFF  /* 从虚拟地址中提取页内偏移量 */              //00000000000000000000111111111111
#define PAGE(addr)		(addr) >> 12  /* 计算页号 */

#define	PAGING_FLAG 		0x80000000	/* CR0 寄存器中启用分页机制的标志位 */ //设置CR0寄存器的最高位为1表示启用分页机制，即将虚拟地址转换为物理地址的过程中将依赖页表。
#define PSE_FLAG 0x00000010	/* 启用页大小扩展的标志位 */


// 定义页面表项中的一些标志位
#define PG_PRESENT	0x00000001	// 页面存在	//设置标记位
#define PG_WRITE	0x00000002	// 可写
#define PG_USER		0x00000004	// 用户级别
#define PG_4MB		0x00000080	// 4MB页面

// 定义页面大小和内存大小
#define PAGESIZE		4096
#define RAM_MAXSIZE		0x100000000 // 4GB
#define RAM_MAXPAGE		0x100000	// 内存总页面数   2^20   一个页大小为2^12  

// 全局描述符表（GDT）中的段描述符结构
struct gdtdesc {
	u16 lim0_15;        // 段限长0-15位
	u16 base0_15;       // 段基址0-15位
	u8 base16_23;       // 段基址16-23位
	u8 acces;           // 访问权限
	u8 lim16_19:4;      // 段限长16-19位
	u8 other:4;         // 其他标志位
	u8 base24_31;       // 段基址24-31位
} __attribute__ ((packed)); // 使用 packed 属性确保结构体紧凑排列

// 全局描述符表寄存器（GDTR）结构
struct gdtr {
	u16 limite;         // 限长
	u32 base;           // 基址
} __attribute__ ((packed));

// 任务状态段（TSS）结构
struct tss {
	u16 previous_task, __previous_task_unused;
	u32 esp0;
	u16 ss0, __ss0_unused;
	u32 esp1;
	u16 ss1, __ss1_unused;
	u32 esp2;
	u16 ss2, __ss2_unused;
	u32 cr3;
	u32 eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	u16 es, __es_unused;
	u16 cs, __cs_unused;
	u16 ss, __ss_unused;
	u16 ds, __ds_unused;
	u16 fs, __fs_unused;
	u16 gs, __gs_unused;
	u16 ldt_selector, __ldt_sel_unused;
	u16 debug_flag, io_map;
} __attribute__ ((packed));

// 中断描述符表（IDT）中的中断门描述符结构
struct idtdesc {
	u16 offset0_15;     // 偏移量0-15位
	u16 select;         // 段选择子
	u16 type;           // 中断类型
	u16 offset16_31;    // 偏移量16-31位
} __attribute__ ((packed));

// 中断描述符表寄存器（IDTR）结构
struct idtr {
	u16 limite;         // 限长
	u32 base;           // 基址
} __attribute__ ((packed));

// 用于保存中断或异常处理过程中的寄存器状态信息的结构
typedef struct {
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32 ds, es, fs, gs;
	u32 which_int, err_code;
	u32 eip, cs, eflags, user_esp, user_ss;
} __attribute__((packed)) regs_t;

// 中断处理函数指针类型
typedef void (*int_desc)(void);

// extern "C" 用于指示编译器按照 C 的方式处理下面的函数声明
extern "C" {
	void init_gdt_desc(u32, u32, u8, u8, struct gdtdesc *);  // 初始化 GDT 描述符
	void init_gdt(void);                                     // 初始化 GDT
	void init_idt_desc(u16, u32, u16, struct idtdesc *);      // 初始化 IDT 描述符
	void init_idt(void);                                     // 初始化 IDT
	void init_pic(void);                                     // 初始化 PIC
	int install_irq(unsigned int num, unsigned int irq);     // 安装中断处理程序
	void switch_to_task(process_st* current, int mode);       // 切换任务
	extern tss default_tss;                                   // 默认 TSS
	u32 cpu_vendor_name(char *name);                          // 获取 CPU 厂商名字
	int dequeue_signal(int);                                  // 信号出队
	int handle_signal(int);                                   // 处理信号
}

#endif


