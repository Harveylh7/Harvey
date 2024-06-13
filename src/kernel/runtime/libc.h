#ifndef LIBC_H
#define LIBC_H

#include <stdarg.h>//用于支持可变参数的函数

extern "C"{
    void itoa(char *buf,unsigned long int n,int base);//无符号长整型数 n 转换为字符串,base表示进制
    void *	memset(char *dst,char src, int n);
    void *	memcpy(char *dst, char *src, int n);
    int 	strlen(char *s);
    int strcmp(const char *dst, char *src);
    int strcpy(char *dst,const char *src);
    void strcat(void *dest,const void*src);//拼接两个字符串
    char *	strncpy(char *destString, const char *sourceString,int maxLength);
	int 	strncmp( const char* s1, const char* s2, int c );
}

#endif