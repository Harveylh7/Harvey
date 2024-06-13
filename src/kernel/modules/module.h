#ifndef __MODULE__
#define __MODULE__

#include <runtime/types.h>
#include <core/file.h>
#include <io.h>


#define NO_FLAG	0//表示没有标志位

class Module
{
	public:
		Module();
		~Module();
		
		void 	initLink();//初始化模块链接。
		void 	init();//初始化模块。
		
		File*	createDevice(char* name,char* module,u32 flag);//创建设备。
		File*	mount(char* dev,char* dir,char* module,u32 flag);//挂载设备到目录。
		File*	install(char* dir,char* module,u32 flag,char* dev);//安装模块。
};

extern Module	modm;

#endif