#include <core/os.h>

//  虚拟内存
extern "C"
{
    char *kern_heap;                       // 这是一个指向内核堆（kernel heap）的指针。内核堆是操作系统内核用来动态分配内存的区域。
    list_head kern_free_vm;                // 用于管理内核虚拟内存的空闲块。
    u32 *pd0 = (u32 *)KERN_PDIR;           // 用于管理虚拟地址与物理地址映射关系的数据结构
    char *pg0 = (char *)0;                 // 表示内核中的第一个页（page0）的起始地址是物理内存的起始位置。
    char *pg1 = (char *)KERN_PG_1;         // 表示内核中的第二个页（page1）的起始地址是物理内存的 4MB 处。
    char *pg1_end = (char *)KERN_PG_1_LIM; // 表示内核中的第二个页（page1）的结束地址是物理内存的 8MB 处。
    u8 mem_bitmap[RAM_MAXPAGE / 8];
    u32 kmalloc_used = 0; // 跟踪当前已分配的内存大小

    // 遍历位图以寻找一个空闲页面，并将其标记为已使用，然后返回其物理地址。
    char *get_page_frame(void)
    {
        int byte, bit;
        int page = -1;

        for (byte = 0; byte < RAM_MAXPAGE / 8; ++byte) // 因为一个字节可以包含8个位所以可以除以8
        {
            if (mem_bitmap[byte] != 0xFF) ////内存位图    0xFF = 1111 1111 ,这个if判断是为了找出有空闲页的比特位
            {
                for (bit = 0; bit < 8; ++bit)
                {
                    if (!(mem_bitmap[byte] & (1 << bit))) // 找到为0的比特位
                    {
                        page = 8 * byte + bit;            // 获得找到的页号
                        set_page_frame_used(page);        // 将页号设置为已经使用
                        return (char *)(page * PAGESIZE); // PAGESIZE 为一个页的大小,根据页号和大小转换为页的地址
                    }
                }
            }
        }
    }

    /*
     * 在内核的虚拟地址空间中查找一个空闲的虚拟页面。然后，函数请求一个空闲的物理页面与之关联。
     * 注意：这些页面位于内核的地址空间中。内核的地址空间将被更新。
     */
    page *get_page_from_heap(void)
    {
        page *pg;
        vm_area *area;
        char *v_addr, *p_addr; // p_addr为页的地址

        p_addr = get_page_frame(); // 获取一个页地址
        if ((int)(p_addr) < 0)
        {
            io.print("PANIC: get_page_from_heap(): no page frame available. System halted !\n");
        }

        // 查看kern_free_m虚拟内存中是否有空闲块
        if (list_empty(&kern_free_vm))
        {
            io.print("PANIC: get_page_from_heap(): not memory left in page heap. System halted !\n");
        }

        // 获取链表中第一个vm_area
        area = list_first_entry(&kern_free_vm, vm_area, list); // list为list_head结构对象，即为前后指针
        v_addr = area->vm_start;                               // v_addr为第一个节点内存开始的地址

        area->vm_start += PAGESIZE; // 节点割下PAGESIZE的地址
        if (area->vm_start == area->vm_end)
        {
            list_del(&area->list); // 改变指针指向来删除节点
            kfree(area);           // 释放空间
        }
        /* 更新内核的地址空间 */
        pd0_add_page(v_addr, p_addr, 0);

        // 新创建的页结构体对象 pg 的双向链表指针成员变量 list 的 next 和 prev 分别初始化为 0。这样就确保了该对象在链表中没有前驱和后继。
        pg = (page *)kmalloc(sizeof(page));
        pg->v_addr = v_addr;
        pg->p_addr = p_addr;
        pg->list.next = 0;
        pg->list.prev = 0;
        return pg; // 返回Page对象
    }

    // 释放页时用到了物理地址
    int release_page_from_heap(char *v_addr)
    {
        struct vm_area *next_area, *prev_area, *new_area;
        char *p_addr;

        /* 找到与 v_addr 关联的页面帧并释放它 */
        p_addr = get_p_addr(v_addr); // 根据虚拟地址找到物理地址

        // 判断找到的物理地址是否为空
        if (p_addr)
        {
            release_page_frame(p_addr); // 先将对应的页从位图中移除
        }
        else
        {
            io.print("WARNING: release_page_from_heap(): no page frame associated with v_addr %x\n", v_addr);
            return 1;
        }

        // 将页目录中的存在标识位取消
        pd_remove_page(v_addr);

        // “更新空闲虚拟地址的列表”
        list_for_each_entry(next_area, &kern_free_vm, list)
        {
            if (next_area->vm_start > v_addr) // 找到第一个起始地址大于v_addr的区域
                break;
        }

        // 上一个节点（vm_area）的起始位置
        prev_area = list_entry(next_area->list.prev, struct vm_area, list); // 地址小于或等于v_addr

        // 这里可以简单的画个图
        if (prev_area->vm_end == v_addr)
        { //   说明v_addr可以变成空闲块插入到链表中，并将prev_area的vm_end指向v_addr的end（合并内存块）
            prev_area->vm_end += PAGESIZE;
            if (prev_area->vm_end == next_area->vm_start)
            {
                prev_area->vm_end = next_area->vm_end; // 注意这里的删除，先将next_area->vm_end附值给prev_area->vm_end防止后续的节点丢失
                list_del(&(next_area->list));          // 修改指针关系
                kfree(next_area);
            }
        }
        else if (next_area->vm_start == v_addr + PAGESIZE)
        { ////如果空闲链表节点相邻则直接将地址赋值给下一个节点
            next_area->vm_start = v_addr;
        }
        else if (next_area->vm_start > v_addr)
        {
            new_area = (struct vm_area *)kmalloc(sizeof(struct vm_area)); // 如果next_area节点与vm_addr并不是连续的则在prev_area后面新增一个节点
            new_area->vm_start = v_addr;
            new_area->vm_end = v_addr + PAGESIZE;
            list_add(&new_area->list, &prev_area->list);
        }
        else
        {
            io.print("\nPANIC: release_page_from_heap(): corrupted linked list. System halted !\n"); // 数据结构被破坏
            asm("hlt");
        }
        return 0;
    }

    // 这段代码用于初始化内存位图并创建内核页目录。
    // 它使用了身份映射（identity mapping）的方式，即虚拟地址等于物理地址，将内核的前4MB映射到同样的虚拟地址空间上。
    // 内存管理
    void Memory_init(u32 high_mem)
    {
        int pg, pg_limit;
        unsigned long i;
        struct vm_area *p;
        struct vm_area *pm;

        /* Numero de la derniere page */
        pg_limit = (high_mem * 1024) / PAGESIZE; // 计算最大‘物理’页数

        /* Initialisation du bitmap de pages physiques */
        // 初始化物理页位图，前部分标记为未使用，后部分标记为已使用
        for (pg = 0; pg < pg_limit / 8; pg++) // 设置可用物理的页数
            mem_bitmap[pg] = 0;
        for (pg = pg_limit / 8; pg < RAM_MAXPAGE / 8; pg++)
            mem_bitmap[pg] = 0xFF;

        /* Pages reservees pour le noyau */
        // 标记内核占用的物理页为已使用状态
        for (pg = PAGE(0x0); pg < (int)(PAGE((u32)pg1_end)); pg++)
        {
            set_page_frame_used(pg);
        }

        /* Initialisation du repertoire de pages */
        // 设置内核的页目录（pd0）
        pd0[0] = ((u32)pg0 | (PG_PRESENT | PG_WRITE | PG_4MB)); //   pd0指向的是页目录的地址
        pd0[1] = ((u32)pg1 | (PG_PRESENT | PG_WRITE | PG_4MB)); //
        for (i = 2; i < 1023; i++)
            pd0[i] = ((u32)pg1 + PAGESIZE * i) | (PG_PRESENT | PG_WRITE); // 后续页表项映射到连续的物理页
        // Page table mirroring magic trick !
        // 页表镜像技巧。在这段代码中，pd0是页目录的指针数组。在x86架构中，页目录最后一个条目用于指向自身，从而形成一个页目录的镜像。这个技巧通常用于操作系统内核，以便让页目录的最后一个条目指向自身，从而可以在不同的内存空间中访问同一个页目录，简化内存管理和访问。
        pd0[1023] = ((u32)pd0 | (PG_PRESENT | PG_WRITE)); // 页目录最后一项指向自身，实现页表的镜像 2^10

        /* Passe en mode pagination */
        // 启用分页机制
        asm("	mov %0, %%eax \n \
	mov %%eax, %%cr3 \n \
	mov %%cr4, %%eax \n \
	or %2, %%eax \n \
	mov %%eax, %%cr4 \n \
	mov %%cr0, %%eax \n \
	or %1, %%eax \n \
	mov %%eax, %%cr0" ::"m"(pd0),
            "i"(PAGING_FLAG), "i"(PSE_FLAG));

        /* Initialisation du heap du noyau utilise par kmalloc */
        // 初始化内核堆
        kern_heap = (char *)KERN_HEAP;
        ksbrk(1); // 分配一额外页给内核堆

        /* Initialisation de la liste d'adresses virtuelles libres */
        // 初始化空闲虚拟地址列表
        p = (struct vm_area *)kmalloc(sizeof(struct vm_area));
        p->vm_start = (char *)KERN_PG_HEAP;
        p->vm_end = (char *)KERN_PG_HEAP_LIM;
        // 将堆的内核堆挂入到kern_free_vm中
        INIT_LIST_HEAD(&kern_free_vm);
        list_add(&p->list, &kern_free_vm);

        // 初始化处理器相关设置
        arch.initProc();
        return;
    }

    // 为一个任务创建和初始化目录的过程。
    struct page_directory *pd_create(void)
    {
        struct page_directory *pd;
        u32 *pdir;
        int i;

        /* Prend et initialise une page pour le Page Directory */
        // 分配并初始化一个用于页目录的页面
        pd = (struct page_directory *)kmalloc(sizeof(struct page_directory));
        pd->base = get_page_from_heap();

        /*
         * Espace kernel. Les v_addr < USER_OFFSET sont adressees par la table
         * de pages du noyau (pd0[]).
         */
        // 内核空间。虚拟地址小于 USER_OFFSET 的地址由内核的页表 (pd0[]) 处理
        pdir = (u32 *)pd->base->v_addr;
        for (i = 0; i < 256; i++) // 内核页表
            pdir[i] = pd0[i];

        /* Espace utilisateur */
        // 用户空间。将页目录的用户空间部分初始化为 0
        for (i = 256; i < 1023; i++) // 用户页表
            pdir[i] = 0;

        /* Page table mirroring magic trick !... */
        // 页表镜像技巧。将页目录的最后一个条目指向自身，实现页表的快速访问
        pdir[1023] = ((u32)pd->base->p_addr | (PG_PRESENT | PG_WRITE));

        /* Mise a jour de la liste des tables de pages de l'espace utilisateur */
        // 初始化用户空间页表列表
        INIT_LIST_HEAD(&pd->pt);

        return pd;
    }

    // 添加一个页表
    void page_copy_in_pd(process_st *current, u32 virtadr)
    {
        struct page *pg;
        pg = (struct page *)kmalloc(sizeof(struct page));
        // 申请一个页,返回的是一个物理地址
        pg->p_addr = get_page_frame();
        // 将虚拟地址 virtadr 向下舍入到页面边界，以获得对应页面的起始地址，并将其赋值给 pg 结构体中的 v_addr 成员，表示页面的虚拟地址。
        pg->v_addr = (char *)(virtadr & 0xFFFFF000);
        // 添加到页面链表中
        list_add(&pg->list, &current->pglist);
        pd_add_page(pg->v_addr, pg->p_addr, PG_USER, current->pd);
    }
    // 页目录的拷贝
    struct page_directory *pd_copy(struct page_directory *pdfather)
    {
        struct page_directory *pd;
        u32 *pdir;
        int i;

        /* 分配并初始化一个新的页目录 */
        pd = (struct page_directory *)kmalloc(sizeof(struct page_directory));
        pd->base = get_page_from_heap();

        /*
         * 内核空间。虚拟地址小于 USER_OFFSET 的地址由内核的页表 (pd0[]) 处理
         */
        pdir = (u32 *)pd->base->v_addr; // 虚拟内存的地址作为页目录的数组
        for (i = 0; i < 256; i++)
        {
            pdir[i] = pd0[i];
        }

        /*
         * 用户空间。从父页目录复制页表
         */
        u32 *father_pdir = (u32 *)pdfather->base->v_addr;
        for (i = 256; i < 1023; i++)
        {
            if (father_pdir[i] & PG_PRESENT) // 页面存在
            {
                if (father_pdir[i] & PG_WRITE)
                {
                    // 分配一个新的页给页表
                    page *new_page_table = get_page_from_heap();
                    // 将父页目录的页表内容复制到新的页表中
                    memcpy((char *)new_page_table, (char *)(father_pdir[i] & 0xFFFFF000), PAGESIZE);
                    // 设置新的页目录条目
                    pdir[i] = ((u32)(new_page_table->p_addr) | (PG_PRESENT | PG_WRITE | PG_USER));
                }
                else
                {
                    pdir[i] = father_pdir[i];
                }
            }
        }

        /* 页表镜像技巧 */
        pdir[1023] = ((u32)pd->base->p_addr | (PG_PRESENT | PG_WRITE));

        /* 初始化用户空间页表列表 */
        INIT_LIST_HEAD(&pd->pt);

        return pd;
    }

    // 返回与传入参数的虚拟地址关联的页面的物理地址
    char *get_p_addr(char *v_addr)
    {
        u32 *pde; // 指向页目录表的指针
        u32 *pte; // 指向页表的指针。

        pde = (u32 *)(0xFFFFF000 | (((u32)v_addr & 0xFFC00000) >> 20)); // FFC为10位，可以找到页目录
        if ((*pde & PG_PRESENT))
        {                                                                              // 找到页目录判断页表是否存在
            pte = (u32 *)(0xFFC00000 | (((u32)v_addr & 0xFFFFF000) >> 10));            // 用最高位的10位拼接到0xFFC00000上找到页目录中的页表
            if ((*pte & PG_PRESENT))                                                   // 判断页表是否存在
                return (char *)((*pte & 0xFFFFF000) + (VADDR_PG_OFFSET((u32)v_addr))); // VADDR_PG_OFFSET为自定义的一个宏，取v_addr虚拟地址的低12位做偏移量
        } // pte中存在的物理地址(20位) + 偏移量的方式找到真实的物理地址
        return 0;
    }

    int pd_destroy(struct page_directory *pd)
    {
        struct page *pg;
        struct list_head *p, *n;

        // 释放每一个页
        list_for_each_safe(p, n, &pd->pt)
        {
            pg = list_entry(p, struct page, list);
            release_page_from_heap(pg->v_addr);
            list_del(p); // 节点 p 所指向的内存将被释放。如果不删除该节点，链表中将会存在悬空指针（dangling pointer），可能导致未定义行为。
            kfree(pg);   // 释放页对象
        }
        // 释放目录的基地址
        release_page_from_heap(pd->base->v_addr);
        kfree(pd);
    }

    // 为内核目录中新加的页设置权限，同时设置set_page_frame_used
    int pd0_add_page(char *v_addr, char *p_addr, int flags) // 内核页目录
    {
        u32 *pde;
        u32 *pte;

        if (v_addr > (char *)USER_OFFSET)
        {
            io.print("ERROR: pd0_add_page(): %p is not in kernel space !\n", v_addr);
            return 0;
        }
        pde = (u32 *)(0xFFFFF000 | (((u32)v_addr & 0xFFC00000) >> 20));
        if ((*pde & PG_PRESENT) == 0)
        {
            // error
        }

        pte = (u32 *)(0xFFC00000 | (((u32)v_addr & 0xFFFFF000) >> 10));
        *pte = ((u32)p_addr) | (PG_PRESENT | PG_WRITE | flags); // 设置权限

        set_page_frame_used(p_addr);
        return 0;
    }

    int pd_add_page(char *v_addr, char *p_addr, int flags, struct page_directory *pd)
    {
        u32 *pde;
        u32 *pte;
        u32 *pt;
        struct page *pg;
        int i;

        pde = (u32 *)(0xFFFFF000 | (((u32)v_addr * 0xFFC00000) >> 20)); // 找到虚拟地址v_addr对应的页目录
        // 如果没有就创建一个页，有的话就直接插入
        if ((*pde & PG_PRESENT) == 0) // 页目录不存在
        {
            pg = get_page_from_heap();
            // 获取新建页表的起始虚拟地址
            pt = (u32 *)pg->v_addr;
            for (i = 1; i < 1024; i++) // 这里是因为页表项有2^10个
            {
                pt[i] = 0;
            }

            *pde = (u32)pg->p_addr | (PG_PRESENT | PG_WRITE | flags); // 设置页目录为存在
                                                                      // 插入pd页目录中
            if (pd)
            {
                list_add(&pg->list, &pd->pt);
            }
        }
        else // 自己修改了
        {
            // 如果存在页表
            // 设置页表的标识位
            pte = (u32 *)(0xFFC00000 | (((u32)v_addr & 0xFFFFF000) >> 10));
            *pte = ((u32)p_addr | (PG_PRESENT | PG_WRITE | flags));
            if (pd)
            {
                list_add(&pg->list, &pd->pt);
            }
        }
        return 0;
    }

    int pd_remove_page(char *v_addr)
    {
        u32 *pte;

        if (get_p_addr(v_addr))
        { // 获取虚拟地址的物理地址
            // 根据页目录找到对应的页, 并将 PG_PRESENT设置取消
            pte = (u32 *)(0xFFC00000 | (((u32)v_addr & 0xFFFFF000) >> 10));
            *pte = (*pte & (~PG_PRESENT));

            asm("invlpg %0" ::"m"(v_addr));
        }
        return 0;
    }
}

// 内核：将一个物理地址 phy 映射到一个虚拟地址 virt，并标记这个映射为用户空间可访问（PG_PRESENT）
void Vmm::kmap(u32 phy, u32 virt)
{
    pd0_add_page((char *)phy, (char *)virt, PG_USER);
}

// 内存的初始化
void Vmm::init(u32 high)
{
    Memory_init(high);
}