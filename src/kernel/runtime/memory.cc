#include <core/os.h>

extern "C"{

    //memcpy 函数用于将 n 个字节从 src 复制到 dst
    void *memcpy(char *dst,char *src,int n)
    {
        char *p = dst;
        while(n--)
            *dst++ = *src++;
        return p;
    }

    void *memset(char *dst,char src,int n)
    {
        char *p = dst;
        while(n--)
            *dst++ = src;
        return p;
    }
}
