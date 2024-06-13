#ifndef BUFFER_H
#define BUFFER_H

#include <runtime/types.h>
class Buffer
{
	public:
		// 构造函数，初始化缓冲区
		Buffer(char* n, u32 siz);
		// 默认构造函数，创建一个空缓冲区
		Buffer();
		// 析构函数，释放缓冲区占用的资源
		~Buffer();
		
		// 向缓冲区添加数据
		void add(u8* c, u32 s);
		// 从缓冲区获取数据
		u32 get(u8* c, u32 s);
		// 清空缓冲区
		void clear();
		// 检查缓冲区是否为空
		u32 isEmpty();
		// 重载右移运算符，从缓冲区中读取数据到指定的字符数组
		Buffer &operator>>(char *c);
		
		// 缓冲区大小
		u32 size;
		// 指向缓冲区数据的指针
		char* map;
	
};



#endif