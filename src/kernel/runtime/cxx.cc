#include <core/os.h>
#include <iostream>

//#ifdef __cplusplus ,注意源文件一定是.cc
extern "C"{
    //这个函数用于注册一个退出时执行的析构函数。参数 Destructor 是一个指向析构函数的指针，参数 Parameter 是传递给析构函数的参数，参数 HomeDSO 是指定与对象相关联的动态共享对象。
    //在程序退出时，注册的析构函数会按照它们注册的顺序被调用。
    int __cxa_atexit(void (*Destructor) (void *), void *Parameter, void *HomeDSO);
    
    //这个函数用于处理程序的终止。它会执行任何已注册的退出时析构函数，然后终止程序。
    void __cxa_finalize(void *);

    //这个函数用于处理纯虚函数的调用。如果在 C++ 类中声明了纯虚函数，而派生类没有实现它，当尝试调用这个纯虚函数时，就会调用到这个函数。
    //默认情况下，这个函数会引发一个纯虚函数调用的异常。
    void __cxa_pure_virtual();

    //这个函数用于设置栈保护的随机数种子。栈保护是一种安全机制，用于检测栈溢出和缓冲区溢出攻击。
    //它会设置一个随机数作为栈保护的种子。
    void __stack_chk_guard_setup();

    //这个函数用于处理栈保护检查失败的情况。如果栈保护检查失败，程序将调用这个函数来处理错误情况。
	//__attribute__((noreturn)) 是 GCC 和 Clang 编译器支持的一种函数属性，用于告诉编译器该函数不会返回。函数 __stack_chk_fail 使用了这个属性，表明调用这个函数之后程序将不会继续执行，而是终止。
    void __attribute__((noreturn)) __stack_chk_fail();
	
    //这个函数用于在 C++ 异常处理期间重新抛出异常。在异常处理程序中，如果需要重新抛出当前异常，可以调用这个函数。
    void _Unwind_Resume();
}

//__dso_handle 是一个全局变量，通常由 C++ 编译器在动态共享对象（DSO，例如共享库或动态链接库）中自动生成，用于管理全局对象的析构顺序。
//用途：用于在程序结束时调用全局对象的析构函数。特别是在 C++ 中，全局对象的析构函数需要在程序结束时被调用，而这些对象可能存在于不同的共享库中。__dso_handle 提供了一种机制，允许库中的全局对象的析构函数在适当的时间被调用。
//定义：在大多数情况下，__dso_handle 是由编译器自动生成的，但你也可以手动定义它。手动定义 __dso_handle 通常用于特殊的链接情况或者需要自定义析构行为时。
void *__dso_handle;	
//__stack_chk_guard 是用于栈保护（stack smashing protection，SSP）的一种机制，通过在栈上放置一个“栈金丝雀”（stack canary）来检测和防止栈溢出攻击。
//用途：在函数执行时，编译器会在栈的特定位置插入一个特殊值（即“金丝雀”），并在函数返回时检查这个值是否被修改。如果值被修改，说明栈溢出攻击可能发生，程序将调用一个失败处理函数（如 __stack_chk_fail）来终止程序。
//定义：__stack_chk_guard 保存了栈金丝雀的值。在大多数系统中，这个值会在程序启动时初始化为一个随机数，以增加安全性。在这个代码中，__stack_chk_guard 被初始化为 0，这是一个不常见的做法，通常用于测试或禁用 SSP 的情况下。

//在函数返回时，系统会检查这个金丝雀值是否被修改，如果被修改，则表示发生了栈溢出，可能是由于缓冲区溢出而引起的攻击。
void *__stack_chk_guard(0);//__stack_chk_guard 被声明为一个指针，初始化为 0。这是一种定义全局变量并初始化的方式。

namespace __cxxabiv1
{
    //这一行定义了一个名为 __guard 的类型，它是一个整型变量。__attribute__((mode(__DI__))) 是 GCC/Clang 编译器的特性，
    //用于指定变量的数据类型。在这里，mode(__DI__) 表示该整型变量的宽度为 64 位（__DI__ 表示 64 位整数）。这个类型通常用于实现 C++ 中的静态初始化保护。
    __extension__ typedef int __guard __attribute__((mode(__DI__)));
    //__extension__ 是 GCC 和一些其他 C/C++ 编译器的扩展标记。它用于告诉编译器，后面的代码中可能会使用到一些非标准的语法或者特性，但是编译器不应该报告这些语法或者特性的警告或者错误。
    //这个标记通常在编写库或者代码时，为了确保代码在不同的编译器中具有相同的行为。因为不同的编译器可能对一些特性的支持程度不同，或者有自己的扩展。使用 __extension__ 可以确保在编译时忽略掉一些编译器特定的警告或者错误。


	extern "C"
	{
        //这个函数用于检查静态初始化保护是否已经被获取。静态初始化保护用于确保静态变量只被初始化一次。
		int __cxa_guard_acquire(__guard *Guard)		{ return ! *(char *) (Guard); }
		//这个函数用于释放静态初始化保护，标记静态变量已经被初始化。
        void __cxa_guard_release(__guard *Guard)	{ *(char *) Guard = 1; }
		//这个函数用于放弃静态初始化保护，表示静态变量的初始化失败。
        void __cxa_guard_abort(__guard *)			{ }
	}
}


// 注册一个在程序退出时执行的析构函数
int __cxa_atexit(void (*) (void *), void *, void *)
{
    // 这个函数什么也不做，总是返回 0 表示成功
    return 0;
}

// 重新抛出异常
void _Unwind_Resume(){
	
}

// 完成 C++ 标准库的销毁工作
void __cxa_finalize(void *)
{
	
}

// 纯虚函数调用
void __cxa_pure_virtual()
{

}

// 设置栈保护的随机数种子
void __stack_chk_guard_setup()
{
    // 将栈保护的随机数设置为特定的值
	unsigned char *Guard;
    //将 Guard 指向 __stack_chk_guard 的地址。__stack_chk_guard 是一个用于栈保护的变量，通常由编译器在代码中插入，用于存储栈保护的随机数种子。
	Guard = (unsigned char *) &__stack_chk_guard;
	Guard[sizeof(__stack_chk_guard) - 1] = 255;//设置格式
	Guard[sizeof(__stack_chk_guard) - 2] = '\n';
	Guard[0] = 0;
}

// 栈保护检查失败时的处理
void __attribute__((noreturn)) __stack_chk_fail()
{
    // 输出错误信息并进入无限循环
	io.print("Buffer Overflow (SSP Signal)\n");
	for(;;) ;
}

// 自定义的内存释放操作符
void operator delete(void *ptr) 
{
	// 调用内核的内存释放函数
		kfree(ptr);
}

// 自定义的内存分配操作符
#ifndef __arm__ // 如果不是 ARM 架构
void* operator new(size_t len) 
{
    // 调用内核的内存分配函数
	return (void*)kmalloc(len);
}

// 自定义的数组形式的内存释放操作符
void operator delete[](void *ptr) 
{
	// 调用普通的内存释放操作符
	::operator delete(ptr);//使用 :: 来明确指定要调用的是全局作用域下的 operator delete 函数，而不是当前命名空间内可能定义的同名函数。
}

// 自定义的数组形式的内存分配操作符
void* operator new[](size_t len) 
{
    // 调用普通的内存分配操作符
	return ::operator new(len);
}

#else // 如果是 ARM 架构

// 自定义的内存分配操作符
void* operator new(size_t len) 
{
    // 调用内核的内存分配函数
	return (void*)kmalloc(len);
}

// 自定义的数组形式的内存释放操作符
void operator delete[](void *ptr) 
{
	// 调用普通的内存释放操作符
	::operator delete(ptr);
}

// 自定义的数组形式的内存分配操作符
void* operator new[](size_t len) 
{
    // 调用普通的内存分配操作符
	return ::operator new(len);
}
#endif


// C++ 异常处理的相关函数
//0xDEADBEAF 是一个常见的“魔数”（Magic Number），在编程中通常用作占位符或特殊标识符，它并不是一个有效的内存地址。
//因此，初始化 __gxx_personality_v0 为这个值是为了暂时占用这个位置，而不是真正的异常处理函数的地址。真正的异常处理函数的地址通常在运行时由编译器或操作系统动态地设置。
void *__gxx_personality_v0=(void*)0xDEADBEAF;
