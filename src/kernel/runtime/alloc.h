
#ifndef ALLOC_H
#define ALLOC_H
//在 C++ 中，函数重载允许多个函数拥有相同的名称，但参数列表不同。这就导致了函数名在目标文件中的重复问题。
//为了解决这个问题，编译器在编译过程中会对函数名进行修饰，即添加一些额外的信息以区分不同的函数。这个过程就叫做名称修饰。

extern "C" {//使用 C 风格的名称，即不进行名称修饰。
	void *ksbrk(int);
	void *kmalloc(unsigned long);
	void kfree(void *);
}

#endif