//POSIX（Portable Operating System Interface）是一个定义了操作系统接口标准的系列规范。
//它旨在提供一个可移植、兼容的操作系统接口，使得应用程序在不同的 UNIX 系统之间可以轻松移植和运行。POSIX 规范定义了一系列系统调用、库函数和命令行工具，以及一些环境变量、文件格式和用户/组管理规范。
#ifndef API_H
#define API_H

// POSIX API 声明

// 文件操作
void call_open();       // 打开文件
void call_close();      // 关闭文件
void call_read();       // 读取文件
void call_write();      // 写入文件
void call_sbrk();       // 调整数据段大小

// I/O 控制操作
void call_ioctl();      // 执行 I/O 控制操作

// 进程操作
void call_exit();       // 退出当前进程
void call_execv();      // 执行新程序
void call_symlink();    // 创建符号链接
void call_getdents();   // 获取目录条目
void call_wait();       // 等待子进程退出
void call_dup2();       // 复制文件描述符
void call_fork();       // 创建新进程
void call_chdir();      // 更改当前目录

// 内存映射操作
void call_mmap();       // 将文件或设备映射到内存

#endif // API_H
