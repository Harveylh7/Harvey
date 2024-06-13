#include <core/os.h>

extern "C"
{

    // 分配n个页大小的内存给内核堆
    void *ksbrk(int n)
    {
        struct kmalloc_header *chunk; // 表示一个分配的内存块的头部信息。
        char *p_addr;
        int i;
        // #define KERN_HEAP			0x10000000
        // kern_heap在Memory_init中已经初始化了
        if ((kern_heap + (n * PAGESIZE)) > (char *)KERN_HEAP_LIM)
        {
            io.print("PANIC: ksbrk(): no virtual memory left for kernel heap !\n");
            return (char *)-1; // 内核常用
        }

        chunk = (struct kmalloc_header *)kern_heap; // 将内核堆的起始地址赋给 chunk


        // 获取相应的页数，并且将其添加到内核的页目录
        for (i = 0; i < n; ++i)
        {
            p_addr = get_page_frame(); // 失败返回值为(char*)-1
            if ((int)(p_addr) < 0)
            {
                io.print("PANIC: ksbrk(): no free page frame available !\n");
                return (char *)-1;
            }

            pd0_add_page(kern_heap, p_addr, 0);

            kern_heap += PAGESIZE; // 移动内核堆的指针
        }
        // 设置内核堆头部信息的属性
        chunk->size = PAGESIZE * n;
        chunk->used = 0;
        return chunk;
    }



    // 分配内核内存块
    void *kmalloc(unsigned long size) // 字节大小
    {
        if (size == 0)
            return 0;

        unsigned long realsize; // 实际分配的内存块大小，包括 kmalloc_header 结构体的大小。
        struct kmalloc_header *chunk, *other;

        if ((realsize = sizeof(struct kmalloc_header) + size) < KMALLOC_MINSIZE) // 如果申请的内存小于KMALLOC_MINSIZE，则令realsize等于KMALLOC_MINSIZE
        {
            realsize = KMALLOC_MINSIZE;//KMALLOC_MINSIZE为16
        }

        // 从内核堆的起始位置()开始，遍历整个堆，寻找一个大小为 'size' 字节的空闲内存块。
        chunk = (struct kmalloc_header *)KERN_HEAP;
        while (chunk->used || chunk->size < realsize) // 其中一个满足就继续找
        {
            if (chunk->size == 0) // 如果发现当前块的大小为 0，表示内存块损坏。打印错误信息并停止系统
            {
                io.print("\nPANIC: kmalloc(): corrupted chunk on %x with null size (heap %x) !\nSystem halted\n",
                         chunk, kern_heap); // 报错并打印地址
                // error
                asm("hit");
                return 0;
            }

            // chunk->size 为内核页n * PAGESIZE
            chunk = (struct kmalloc_header *)((char *)chunk + chunk->size);

            if (chunk == (struct kmalloc_header *)kern_heap) // 已经到内核堆的顶部了,还没找到有用的页就分配页
            {
                if ((int)(ksbrk((realsize / PAGESIZE) + 1)) < 0)
                { // 尝试再为内核堆分配页，+1是为了分配一个完整的页，如果小于一个页就变成一页
                    io.print("\nPANIC: kmalloc(): no memory left for kernel !\nSystem halted\n");
                    asm("hlt");
                    return 0;
                }
            }
            else if ((chunk > (struct kmalloc_header *)kern_heap)) // 检测内核堆内存管理中的严重错误
            {
                io.print("\nPANIC: kmalloc(): chunk on %x while heap limit is on %x !\nSystem halted\n",
                         chunk, kern_heap);
                asm("hlt");
                return 0;
            }
        }
        // while循环外说明已经找到了未使用的内核页（>=size），我们将对内核页进行切分
        if (chunk->size - realsize < KMALLOC_MINSIZE) // 如果找到的页块与实际的需要的大小相差小于KMALLOC_MINSIZE则直接可以设置使用
        {
            chunk->used = 1;
        }
        else
        {
            // 如果相差较大就将差值赋值给other
            other =
                (struct kmalloc_header *)((char *)chunk + realsize); // 地址进行强转
            other->size = chunk->size - realsize;
            other->used = 0;
            // 将真实需要的大小赋值给chunk
            chunk->size = realsize;
            chunk->used = 1;
        }
        kmalloc_used += realsize;

        /* Return a pointer to the memory area */
        return (char *)chunk + sizeof(struct kmalloc_header); // 由于是堆所以需要矫正指针，+struct kmalloc_header指向该堆的起始位置
    }


    // 释放内存
    void kfree(void *v_addr)
    {
        if (v_addr == (void *)0)
        {
            return;
        }
        struct kmalloc_header *chunk, *other;

        // 获取堆的起始地址
        chunk = (kmalloc_header *)((u32)v_addr - sizeof(kmalloc_header));

        // 设置头部信息为未使用
        chunk->used = 0;

        // 设置总体内核的使用数量
        kmalloc_used -= chunk->size;

        //如果后面有未使用的内存块，则将其合并并设置块大小（也就是设置了分配的内存大小）
        while ((other =
                    (struct kmalloc_header *)((char *)chunk + chunk->size)) &&
               other < (struct kmalloc_header *)kern_heap && other->used == 0)
            chunk->size += other->size;
    }
}
