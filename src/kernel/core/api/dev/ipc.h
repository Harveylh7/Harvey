//进程间通信模块

#ifndef __API_IPC__
#define __API_IPC__
// 标准IPC文件描述符
#define STDIPC_FILENO   3

// IPC信号
#define SIGIPC  SIGUSR1

// API调用：切换TTY屏幕
#define API_TTY_SWITCH_SCREEN   0xff52 //tty.h中也有

#endif
