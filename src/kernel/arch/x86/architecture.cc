// 在内核开发中，采用结构体直接存储数据有许多重要的原因，主要包括以下几点：

// 1. 数据组织和管理
// 结构化数据：
// 结构体允许将相关的数据组合在一起，形成一个逻辑上的单元。这样可以更清晰地表示和管理复杂的数据结构。例如，进程控制块（Process Control Block, PCB）中包含了进程的所有相关信息，如寄存器状态、页表地址、文件描述符等，通过一个结构体来管理这些信息，可以使代码更加整洁和易于维护。

// 2. 内存布局和性能
// 内存连续性：
// 结构体内的数据成员在内存中是连续存储的，这种连续性可以提高数据访问的效率，因为在访问结构体的不同成员时，可以减少缓存未命中的概率，从而提升性能。

// 缓存友好：
// 现代处理器的缓存设计依赖于数据的局部性。结构体的成员通常在逻辑上是相关的，访问一个成员之后很可能会访问另一个成员。将这些成员放在连续的内存位置上，可以提高缓存命中率，减少访问内存的延迟。

// 3. 可维护性和可读性
// 代码清晰：
// 使用结构体可以使代码更加直观和易读。比如，通过process->state就可以很清楚地知道这是在访问进程的状态，而不是其他不相关的变量。

// 封装性：
// 结构体有助于封装相关的数据，减少全局变量的使用。这种封装性提高了代码的可维护性，减少了变量命名冲突的风险。

// 4. 类型安全和编译时检查
// 类型安全：
// 结构体为不同的数据类型提供了明确的定义，编译器可以进行类型检查，防止类型不匹配的错误。例如，结构体中的成员类型是明确的，编译器可以在编译时检查对这些成员的访问是否合法。

// 5. 易于扩展
// 扩展性：
// 结构体允许在不改变现有代码的情况下添加新的成员。比如，在内核开发中，如果需要为进程添加新的属性，只需要在进程控制块的结构体中增加一个新的成员即可，而不需要修改大量的代码。

// 6. 内核对象管理
// 对象化管理：
// 内核中有很多对象需要管理，如进程、线程、文件、设备等。这些对象的属性和状态通常通过结构体来描述。通过结构体，内核可以更加方便地管理和操作这些对象。


#include <core/os.h>
#include <x86.h>

extern u32 *stack_ptr; // 栈指针

// CPU名称
static char cpu_name[512] = "x86-noname";

// 检测CPU类型并返回CPU名称
char *Architecture::detect()
{
    cpu_vendor_name(cpu_name);
    return cpu_name;
}

// 开始并初始化系统架构

void Architecture::init()
{
    // 调用 detect 获取并打印CPU名称。
    // 加载GDT（全局描述符表）。
    // 设置栈段寄存器（SS）和栈指针（ESP）。
    // 加载IDT（中断描述符表）。
    // 配置PIC（可编程中断控制器）。
    // 加载任务寄存器（TR）。
    io.print("Architecture x86, cpu=%s \n", detect());

    io.print("Loading GDT \n");
    init_gdt();
    asm("	movw $0x18, %%ax \n \
			movw %%ax, %%ss \n \
			movl %0, %%esp" ::"i"(KERN_STACK));

    io.print("Loading IDT \n");
    init_idt();

    io.print("Configure PIC \n");
    init_pic();

    io.print("Loading Task Register \n");
    asm("	movw $0x38, %ax; ltr %ax");
}

// 初始化第一个进程：内核进程
void Architecture::initProc()
{
    firstProc = new Process("kernel");
    firstProc->setState(ZOMBIE); // 将进程设置为僵尸进程
    
    //添加多个终端
    firstProc->addFile(fsm.path("/dev/tty"),0);
    firstProc->addFile(fsm.path("/dev/tty"),0);
    firstProc->addFile(fsm.path("/dev/tty"),0);

    plist = firstProc; //plist为Architecture的进程链表
    pcurrent=firstProc; //设置Architecture中的当前进程为firstProc
    pcurrent->setPNext(NULL);//设置这个进程的下一个进程为NULL
    process_st* current = pcurrent->getPInfo();
    current->regs.cr3 = (u32) pd0;//CR3 寄存器是 x86 架构中的控制寄存器，存储着页目录表的物理地址。
}

//重新启动计算机的函数
void Architecture::reboot(){
    u8 good = 0x02;
    while ((good & 0x02) != 0)
        good = io.inb(0x64);//从0x64中读取一个字节,如果为0则表示计算机准备好重新启动
    io.outb(0x64, 0xFE);
    // 使用一个 while 循环，条件是 (good & 0x02) != 0，即循环会一直执行直到 good 的第二位（从右往左数）被清零，也就是直到 good 变量的第二位变为0。
    // 在循环体内部，调用 io.inb(0x64) 函数从端口 0x64 读取一个字节的数据，该端口通常用于与计算机的 PS/2 键盘控制器进行通信。
    // 然后将读取的数据与 0x02 进行按位与运算，以检查 good 变量的第二位是否为1。
    // 如果 good 变量的第二位为1，表示计算机尚未准备好重新启动，因此程序会继续等待。如果第二位为0，表示计算机已经准备好重新启动，程序会继续执行下一步。
    // 在循环外部，调用 io.outb(0x64, 0xFE) 函数，向端口 0x64 发送 0xFE 命令，这是一种向计算机发送重新启动信号的方法。
}


/* Shutdown the computer */
void Architecture::shutdown(){
	// todo
    //在操作系统中，要实现安全的系统关机需要执行一系列复杂的操作，例如保存当前系统状态、关闭所有运行中的进程、释放系统资源等。这些操作通常需要通过与硬件进行交互来完成。
}

/* Install a interruption handler */
void Architecture::install_irq(int_handler h){
	// todo
    //在操作系统中，安装中断处理程序通常涉及到与中断控制器进行通信，向中断描述符表 (IDT) 中注册中断处理程序的地址，并设置中断向量等操作。由于这些操作需要与底层硬件进行交互，因此实现起来相对复杂。
}

//将进程添加到调度程序
void Architecture::addProcess(Process* p){
    //头插法,插入新的进程
    p->setPNext(plist);
    plist =p;
}

//创建一个进程,将父进程的信息拷贝给子进程
int Architecture::fork(process_st* info,process_st* father){
    memcpy((char*)info,(char*)father,sizeof(process_st));
	info->pd = pd_copy(father->pd);

    
    

    info->parent = father;
    // 在这里我们要返回给父进程子进程的 PID，返回给子进程 0
    if (is_current_process_child()) {
        // 如果是子进程
        return 0;
    } else {
        // 如果是父进程
        return info->pid;
    }

}
bool Architecture::is_current_process_child() {
    // 这个函数的实现依赖于操作系统的架构和进程管理机制
    // 在这里做一个简单的示例
    return pcurrent->getPid()== 0;
}

int Architecture::createProc(process_st* info, char* file, int argc, char** argv)
{
    page *kstack;
    process_st *previous;
    process_st *current;

    char **param, **uparam;
    u32 stackp;
    u32 e_entry;

    int pid;
    int i;

    pid =1;//PID为1的进程通常是init进程。init进程是系统引导时最先启动的进程，它负责初始化系统环境、启动各种系统服务和守护进程，并且监控系统中的其他进程。
    info->pid = pid;

    if(argc){//如果有参数传递给新进程，就为这些参数分配内存，并将参数复制到新进程的内存空间中。
        param = (char**) kmalloc(sizeof(char*)*(argc+1));//存储char* 的数组
        for(i=0; i <argc;++i)
        {
            //开好空间，并将参数传递给param数组中
            param[i] = (char*) kmalloc(strlen(argv[i]) + 1);
            strcpy(param[i],argv[i]);
        }
        //数组结尾赋值0，这是一般操作系统存储环境变量的方式（以0结尾）
        param[i]=0;
    }
    //添加页目录的信息
    info->pd = pd_create();
    //初始化进程链表
    INIT_LIST_HEAD(&(info->pglist));

    previous = arch.pcurrent->getPInfo();//获取当前进程的信息并赋值给previous
	current=info;//将传来的进程信息传递给current
	
    asm("mov %0, %%eax; mov %%eax, %%cr3"::"m"((info->pd)->base->p_addr));//获取参数info中页目录基页中的物理地址
	
    e_entry = (u32) load_elf(file,info);//获取文件开始的指针

    if(e_entry == 0)//解析失败就恢复之前的状态
    {
        //清空环境变量
        for(i=0;i<argc;++i)
            kfree(param[i]);
        kfree(param);

		arch.pcurrent = (Process*) previous->vinfo;//将先前虚拟地址赋值给pcurrent
		current=arch.pcurrent->getPInfo();//将信息重新赋值给current
		asm("mov %0, %%eax ;mov %%eax, %%cr3"::"m" (current->regs.cr3));//使用内联汇编将当前进程的页目录基地址重新加载到 cr3 寄存器，恢复原来进程的页表。
		pd_destroy(info->pd);//销毁创建的页目录
		return -1;
    }
    stackp =  USER_STACK - 16;//将栈指针向下移动16字节
    
    //将环境变量存入栈
    if(argc){
        uparam = (char**) kmalloc(sizeof(char*)*argc);

        for(i=0;i<argc;++i)
        {
            //将环境变量存入栈区地址
            stackp -= (strlen(param[i])+1);//“\0”
            strcpy((char*)stackp,param[i]);
            uparam[i] = (char*)stackp;//将stackp指向的内容也存入uparam
        }

        stackp &= 0xFFFFFFF0;//将地址向下对齐到16字节边界

        //创建 main() 函数的参数：argc 和 argv[]
        stackp -= sizeof(char*);
        *((char**)stackp)=0;//将该二维数组的起始位置先初始化为0

        for(i=argc-1;i>=0;--i)//压栈的顺序是反的
        {
            stackp -= sizeof(char*);
            *((char**)stackp) = uparam[i];
        }


        //这一步存储 argv 数组的起始地址//因为是32位，指针大小为4字节
		stackp -= sizeof(char*);	
		*((char**) stackp) = (char*) (stackp + 4); 

		stackp -= sizeof(char*);	
		*((int*) stackp) = argc; 

		stackp -= sizeof(char*);

		for (i=0 ; i<argc ; i++) 
			kfree(param[i]);

        //利用栈指针存入物理地址后就可以释放这两个临时变量了
		kfree(param);
		kfree(uparam);
    }

    kstack = get_page_from_heap();//为新进程获取一个内核页


    // Initialise le reste des registres et des attributs 
	info->regs.ss = 0x33;       //设置用户模式的数据段选择子
	info->regs.esp = stackp;    //设置用户栈指针为已准备好的 stackp。ESP 是 x86 架构中的栈指针寄存器。它指向当前栈顶的地址，
	info->regs.eflags = 0x0;    //清除标志寄存器。
	info->regs.cs = 0x23;       //设置代码段选择子为用户模式。
	info->regs.eip = e_entry;   //设置指令指针为程序入口点 e_entry
	//设置数据段选择子。
    info->regs.ds = 0x2B;       
	info->regs.es = 0x2B;
	info->regs.fs = 0x2B;
	info->regs.gs = 0x2B;
    //设置页目录基址寄存器为新进程的页表地址。
	info->regs.cr3 = (u32) info->pd->base->p_addr;

    //kstack为内核栈信息
	info->kstack.ss0 = 0x18;//设置内核模式的数据段选择子
    
    //因为栈在 x86 架构中是向下增长的，所以我们需要从页面的顶部开始使用栈。
	//当进程从用户态切换到内核态时，CPU 会自动使用 esp0 作为内核栈的栈顶指针。
    info->kstack.esp0 = (u32) kstack->v_addr + PAGESIZE - 16;//设置内核栈指针为刚分配的内存页的顶部。(再减去 16 字节，可能是以便为将来栈操作预留一些空间。)


    //清空通用寄存器
	info->regs.eax = 0;
	info->regs.ecx = 0;
	info->regs.edx = 0;
	info->regs.ebx = 0;

	info->regs.ebp = 0;
	info->regs.esi = 0;
	info->regs.edi = 0;

	info->b_heap = (char*) ((u32) info->e_bss & 0xFFFFF000) + PAGESIZE;//+PAGESIZE能找到堆的起始地址
	info->e_heap = info->b_heap;//初始化堆时堆的开始地址和结尾地址相同

	info->signal = 0;//所有信号初始化为0
	for(i=0 ; i<32 ; i++)
		info->sigfn[i] = (char*) SIG_DFL;//初始化所有信号为默认处理方法


    //恢复进程的上下文
	arch.pcurrent = (Process*) previous->vinfo;
	current=arch.pcurrent->getPInfo();
	asm("mov %0, %%eax ;mov %%eax, %%cr3":: "m"(current->regs.cr3));
	
	return 1;
}


void Architecture::destroy_process(Process* pp)
{
    disable_interrupt();

    u16 kss;
    u32 kesp;
    u32 accr3;
    list_head *p, *n;
    page *pg;
    //获取当前运行进程的信息
    process_st *proccurrent = (arch.pcurrent)->getPInfo();
    process_st *pidproc=pp->getPInfo();//获取传入的进程信息

    //这段内联汇编代码通过切换 cr3 寄存器来切换页表，使得后续操作能够在 pidproc 进程的地址空间内进行。
    //主要用于内存管理单元（MMU）进行地址转换。具体来说，它存储当前使用的页目录的基地址，从而使得 CPU 可以正确地将虚拟地址转换为物理地址。
    asm("mov %0, %%eax ;mov %%eax, %%cr3"::"m" (pidproc->regs.cr3));



    //释放内存：
    //释放可执行代码使用的页:
    //释放用户栈:
    //释放内核栈:
    //释放页目录:
    list_for_each_safe(p, n, &pidproc->pglist) {//释放进程中的页
		pg = list_entry(p, struct page, list);
		release_page_frame(pg->p_addr);
		list_del(p);
		kfree(pg);
	}

    //kstack = get_page_from_heap();前面创建进程时创建了一个内核页
    //释放内核页
    release_page_from_heap((char *) ((u32)pidproc->kstack.esp0 & 0xFFFFF000));

    //释放页目录
    asm("mov %0, %%eax; mov %%eax, %%cr3"::"m"(pd0));

    pd_destroy(pidproc->pd);

    asm("mov %0, %%eax ;mov %%eax, %%cr3"::"m" (proccurrent->regs.cr3));//cr3 存储的是页目录的基地址
	

    //将该进程从架构结构体的进程链表中移除
    if(plist==pp)//如果头为要删除的节点则plist直接指向下一个节点
    {
        plist = pp->getPNext();
    }else{
        //遍历指针
        Process* l=plist;
		Process*ol=plist;
		while (l!=NULL){
			if (l==pp){
                //指向下一个进程来删除节点
				ol->setPNext(pp->getPNext());
			}
            //更新指针
			ol=l;
			l=l->getPNext();
		}
    }
    //使能中断
    enable_interrupt();
}
void Architecture::change_process_father(Process* pe,Process* pere){
    Process* p=plist;//获取当前的进程
	Process* pn=NULL;
	while (p!=NULL){
		pn=p->getPNext();//pn指向p的下一个
		if (p->getPParent()==pe){
			p->setPParent(pere);
		}
		p=pn;
	}
}

//删除 init 进程会导致严重的系统不稳定甚至崩溃，因为它是系统中非常关键的进程。
//因此，在检查并销毁僵尸进程时，必须确保不会误删除 init 进程。通过添加 p->getPid() != 1 这一条件，可以避免删除 init 进程，确保系统的正常运行。
void Architecture::destroy_all_zombie(){//消灭所有的僵尸进程
	Process* p=plist;
	Process* pn=NULL;
	while (p!=NULL){
		pn=p->getPNext();
		if (p->getState()==ZOMBIE && p->getPid()!=1){
			destroy_process(p);
			delete p;
		}
		
		p=pn;
	}
}

// 设置寄存器的值的函数
void Architecture::setParam(u32 ret,u32 ret1,u32 ret2,u32 ret3,u32 ret4){
    ret_reg[0]=ret;
	ret_reg[1]=ret1;
	ret_reg[2]=ret2;
	ret_reg[3]=ret3;
	ret_reg[4]=ret4;
}


//使能中断
void Architecture::enable_interrupt(){
    asm("sti");//sti 指令会将 CPU 的中断标志位（IF，Interrupt Flag）设置为 1，从而允许处理器响应外部硬件中断。
}


/* Disable the interruption */
//屏蔽中断
void Architecture::disable_interrupt(){
	asm ("cli");//cli 指令会将 CPU 的中断标志位（IF，Interrupt Flag）设置为 0，从而禁用处理器响应外部硬件中断。cli 指令会将 CPU 的中断标志位（IF，Interrupt Flag）设置为 0，从而禁用处理器响应外部硬件中断。
}


/* Get a syscall argument */
//得到系统调用参数
u32	Architecture::getArg(u32 n){
	if (n<5)
		return ret_reg[n];
	else
		return 0;
}


/* Set the return value of syscall */
//设置系统调用的返回值
void Architecture::setRet(u32 ret){
	stack_ptr[14] = ret;
}