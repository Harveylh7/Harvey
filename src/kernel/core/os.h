#ifndef OS_H
#define OS_H

#include <kernel/config.h>
#include <core/kernel.h>



//设备驱动程序的函数指针
typedef File* (*device_driver) (char* name,u32 flag,File* dev);//函数指针

//模块类
struct module_class{
	int					module_type;
	char*			module_name;
	char*			class_name;
	device_driver	drive;
};






//与模块相关的宏
#define MODULE_DEVICE		0
#define MODULE_FILESYSTEM	1
//利用宏自定义模块的名字
#define module(name,type,classe,mknod)	module_class classe##_module={type,\    
																name, \
																#classe, \
																(device_driver)mknod};	
//利用宏自定义模块的名字
#define import_module(classe) 	extern module_class  classe##_module

//开始定义一个模块数组。
#define run_module_builder	module_class* module_builder[]=
#define build_module(classe) 	&classe##_module//将模块添加到数组中。
#define end_module() 		NULL                //标记模块数组的结束。

#define std_buildin_module	void Module::init()//定义模块初始化函数的开始。
#define	run_module(n,m,f) createDevice(#m,#n,f);//运行一个模块，并创建一个设备   #能将宏转化为字符串

/*
 *	Asm Macro
 */
#define asm 		__asm__                 //内联汇编代码。
#define asmv 		__asm__ __volatile__    //防止编译器优化掉这些代码。

//__asm__ __volatile__ 是 GCC（GNU Compiler Collection）中的内联汇编指令，
//用于在 C 或 C++ 代码中嵌入汇编代码。它的作用是让编译器直接在生成的机器代码中插入指定的汇编指令，
//并且通过 __volatile__ 修饰符告知编译器不要对这些汇编指令进行优化或重排序。

//__asm__ __volatile__ ("assembly code" : output operands : input operands : clobbered registers);

//1. "assembly code"
//这是要插入的汇编指令，可以包含占位符，这些占位符用 %0, %1, 等表示，表示操作数的顺序。

//2. output operands
//输出操作数，描述如何将汇编指令的结果传递回 C 变量

//3. input operands
//输入操作数，描述从 C 变量传递到汇编指令的输入。

//4. clobbered registers
//列出在汇编代码中可能被修改的寄存器，通知编译器这些寄存器在执行汇编代码后需要被重新加载。









#endif