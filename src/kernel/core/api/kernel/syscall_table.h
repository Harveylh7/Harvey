//用于内核的系统调用分发器，根据传递的编号来执行相应的系统调用。


#ifndef _OS_SYSCALL_TABLE_H_
#define _OS_SYSCALL_TABLE_H_

#define NOT_DEFINED 0  // 表示未定义的系统调用

// 系统调用号的枚举
enum {
    SYS_rewinddir = NOT_DEFINED,       // 重置目录流位置
    SYS_sbrk = 45,                     // 增加数据段大小 (brk系统调用)
    SYS_fork = NOT_DEFINED,            // 创建新进程 (fork系统调用)
    SYS_write = 4,                     // 写入文件描述符 (fd, buffer, count)
    SYS_read = 3,                      // 从文件描述符读取 (fd, buffer, count)
    SYS_open = 5,                      // 打开文件 (filename, flag)
    SYS_close = 6,                     // 关闭文件描述符 (fd)
    SYS_execve = 11,                   // 执行程序 (filename, argv, envp)
    SYS_dup = NOT_DEFINED,             // 复制文件描述符
    SYS_dup2 = 38,                     // 复制文件描述符到另一个描述符
    SYS_pwrite = NOT_DEFINED,          // 在给定偏移量写入文件描述符
    SYS_pread = NOT_DEFINED,           // 在给定偏移量读取文件描述符
    SYS_exit = 1,                      // 终止调用进程 (status)
    SYS_getdents = 89,                 // 获取目录条目
    SYS_fchdir = NOT_DEFINED,          // 更改文件描述符的工作目录
    SYS_isatty = NOT_DEFINED,          // 检查文件描述符是否指向终端
    SYS_lseek = 19,                    // 重新定位读/写文件偏移量
    SYS_unlink = 17,                   // 删除文件名及其可能指向的文件
    SYS_link = 18,                     // 为文件创建新名字
    SYS_readlink = 19,                 // 读取符号链接的值
    SYS_sleep_thread = NOT_DEFINED,    // 使线程休眠
    SYS_access = NOT_DEFINED,          // 检查用户对文件的权限
    SYS_chdir = 12,                    // 更改工作目录
    SYS_getpid = 20,                   // 获取进程ID
    SYS_getuid = 70,                   // 获取用户ID
    SYS_gettid = NOT_DEFINED,          // 获取线程ID
    SYS_rmdir = NOT_DEFINED,           // 删除目录
    SYS_symlink = 9,                   // 创建符号链接 (oldname, newname)
    SYS_fcntl = NOT_DEFINED,           // 文件控制操作
    SYS_get_system_time = NOT_DEFINED, // 获取系统时间
    SYS_stat = 106,                    // 获取文件状态
    SYS_fstat = NOT_DEFINED,           // 获取打开文件的状态
    SYS_stime = NOT_DEFINED,           // 设置系统时间
    SYS_mkdir = 15,                    // 创建目录
    SYS_ioctl = 54,                    // I/O控制操作 (fd, address, buffer)
    SYS_select = NOT_DEFINED,          // 监视多个文件描述符
    SYS_mount = 13,                    // 挂载文件系统
    SYS_unmount = 14,                  // 卸载文件系统
    SYS_lstat = NOT_DEFINED,           // 获取文件状态 (符号链接)
    SYS_utime = NOT_DEFINED,           // 更改文件的最后访问和修改时间
    SYS_wait4 = 7,                     // 等待进程状态改变
    SYS_socket = NOT_DEFINED,          // 创建通信端点
    SYS_connect = NOT_DEFINED,         // 在套接字上发起连接
    SYS_sigaction = 67,                // 检查和更改信号处理程序
    SYS_kill = 37,                     // 向进程发送信号
    SYS_sigprocmask = NOT_DEFINED,     // 检查和更改阻塞的信号
    SYS_dbprintf = NOT_DEFINED,        // 打印调试信息
    SYS_create_semaphore = NOT_DEFINED,// 创建信号量
    SYS_delete_semaphore = NOT_DEFINED,// 删除信号量
    SYS_lock_semaphore = NOT_DEFINED,  // 锁定信号量
    SYS_unlock_semaphore = NOT_DEFINED,// 解锁信号量
    SYS_create_thread = 101,           // 创建新线程
    SYS_wake_up_thread = NOT_DEFINED,  // 唤醒休眠线程
    SYS_kill_thread = NOT_DEFINED,     // 终止线程
    SYS_mmap = 55,                     // 将文件或设备映射到内存
    
    SYS_loadmod = 71,                  // 加载内核模块
    SYS_login = 72,                    // 用户登录
    SYS_newuser = 73,                  // 创建新用户
};

#endif
