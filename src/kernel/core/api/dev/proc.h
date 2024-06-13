#ifndef __API_PROC__
#define __API_PROC__

struct proc_info//进程信息
{
    char				name[32];//存储进程名称
	unsigned int		pid;//存储进程ID
	unsigned int		tid;//存储线程ID
	unsigned char		state;//存储进程状态
	unsigned int		vmem;//存储进程的虚拟内存的大小
	unsigned int		pmem;//存储进程的物理内存的大小
};

enum{//定义了进程的状态
    PROC_STATE_RUN =0,
    PROC_STATE_ZOMBIE=1,
    PROC_STATE_THREAD=2
};

#define API_PROC_GET_PID   0x5200   //用于表示获取进程ID的API编号。
#define API_PROC_GET_INFO  0x5201   //用于表示获取进程信息的API编号。

#endif