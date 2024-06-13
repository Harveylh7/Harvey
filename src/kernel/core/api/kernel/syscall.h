#ifndef _OS_SYSCALL_H_
#define _OS_SYSCALL_H_

/* 声明不带参数的系统调用函数 */
int syscall0(int number);

/* 声明带一个参数的系统调用函数 */
int syscall1(int number, unsigned int p1);

/* 声明带两个参数的系统调用函数 */
int syscall2(int number, unsigned int p1, unsigned int p2);

/* 声明带三个参数的系统调用函数 */
int syscall3(int number, unsigned int p1, unsigned int p2, unsigned int p3);

/* 声明带四个参数的系统调用函数 */
int syscall4(int number, unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4);

/* 声明带五个参数的系统调用函数 */
int syscall5(int number, unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned int p5);

#endif
