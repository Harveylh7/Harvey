#ifndef ARCH_H
#define ARCH_H

#include <runtime/types.h>

#include <process.h>


//处理器架构类
class Architecture
{
	public:
		/** architecture class functions **/
		void	init();			//启动处理器接口
		void	reboot();		//重新启动计算机
		void	shutdown();		//关闭计算机
		char*	detect();		//检测处理器类型
		void	install_irq(int_handler h);	//安装一个中断处理程序
		void	addProcess(Process* p);		//将一个进程添加到调度器
		void	enable_interrupt();		//启用中断
		void	disable_interrupt();	//禁用中断
		int 	createProc(process_st* info,char* file,int argc,char** argv);	//初始化一个程序
		void	setParam(u32 ret,u32 ret1,u32 ret2, u32 ret3,u32 ret4);		//设置系统调用的参数
		u32		getArg(u32 n);		//获取系统调用参数
		void	setRet(u32 ret);	//设置系统调用的返回值
		void 	initProc();			//初始化进程链表
		void	destroy_process(Process* pp);	//销毁一个进程
		void	destroy_all_zombie();//销毁所有僵尸进程
		void	change_process_father(Process* p,Process* pere);//改变进程的父进程
		int		fork(process_st* info,process_st* father);	//创建子进程
		
		bool Architecture::is_current_process_child();
		
		/** architecture public class attributes */
		Process*	pcurrent;		//当前进程
		Process*	plist;			//进程链表
		
		
	private:
		
		u32			ret_reg[5];//系统调用返回值寄存器数组
		Process* 	firstProc;//第一个进程
		
};

//定义了一个全局的 Architecture 类型的对象 arch，用于表示处理器架构的实例
extern Architecture arch;

#endif