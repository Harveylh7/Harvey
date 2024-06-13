#ifndef SIGNAL_H
#define SIGNAL_H

#include <runtime/types.h>
#include <process.h>

#define SIGHUP 1       /* 挂起 (POSIX).  */
#define SIGINT 2       /* 中断 (ANSI).  */
#define SIGQUIT 3      /* 退出 (POSIX).  */
#define SIGILL 4       /* 非法指令 (ANSI).  */
#define SIGTRAP 5      /* 跟踪陷阱 (POSIX).  */
#define SIGABRT 6      /* 中止 (ANSI).  */
#define SIGIOT 6       /* IOT陷阱 (4.2 BSD).  */
#define SIGBUS 7       /* 总线错误 (4.2 BSD).  */
#define SIGFPE 8       /* 浮点异常 (ANSI).  */
#define SIGKILL 9      /* 杀死，不能被阻塞 (POSIX).  */
#define SIGUSR1 10     /* 用户定义信号1 (POSIX).  */
#define SIGSEGV 11     /* 段错误 (ANSI).  */
#define SIGUSR2 12     /* 用户定义信号2 (POSIX).  */
#define SIGPIPE 13     /* 管道破裂 (POSIX).  */
#define SIGALRM 14     /* 闹钟 (POSIX).  */
#define SIGTERM 15     /* 终止 (ANSI).  */
#define SIGSTKFLT 16   /* 栈故障.  */
#define SIGCLD SIGCHLD /* 与SIGCHLD相同 (System V).  */
#define SIGCHLD 17     /* 子进程状态改变 (POSIX).  */
#define SIGCONT 18     /* 继续 (POSIX).  */
#define SIGSTOP 19     /* 停止，不可被阻塞 (POSIX).  */
#define SIGTSTP 20     /* 键盘停止 (POSIX).  */
#define SIGTTIN 21     /* 后台从tty读 (POSIX).  */
#define SIGTTOU 22     /* 后台向tty写 (POSIX).  */
#define SIGURG 23      /* 套接字紧急情况 (4.2 BSD).  */
#define SIGXCPU 24     /* CPU时间限制超出 (4.2 BSD).  */
#define SIGXFSZ 25     /* 文件大小限制超出 (4.2 BSD).  */
#define SIGVTALRM 26   /* 虚拟闹钟 (4.2 BSD).  */
#define SIGPROF 27     /* 分析计时器 (4.2 BSD).  */
#define SIGWINCH 28    /* 窗口大小改变 (4.3 BSD, Sun).  */
#define SIGPOLL SIGIO  /* 可轮询事件发生 (System V).  */
#define SIGIO 29       /* I/O现在可能 (4.2 BSD).  */
#define SIGPWR 30      /* 电源故障重启 (System V).  */
#define SIGSYS 31      /* 错误的系统调用.  */

#define SIGEVENT SIGPOLL /* 自定义信号：事件信号 */
#define SIGIPC SIGUSR1   /* 自定义信号：进程间通信信号 */

#define SIG_DFL 0 /* 默认信号处理 */
#define SIG_IGN 1 /* 忽略信号 */

/* 设置信号位：将指定信号的位设置为1 */
#define set_signal(mask, sig) *(mask) |= ((u32)1 << (sig - 1))
/* 清除信号位：将指定信号的位清除为0 */
#define clear_signal(mask, sig) *(mask) &= ~((u32)1 << (sig - 1))
/* 检查信号位：检查指定信号的位是否被设置 */
#define is_signal(mask, sig) (mask & ((u32)1 << (sig - 1)))

#endif