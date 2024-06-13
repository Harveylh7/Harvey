#ifndef CONFIG_H
#define CONFIG_H

#define KERNEL_NAME		"devos"		/* kernel name */
#define KERNEL_VERSION	"1"		/* kernel version */
#define KERNEL_DATE		__DATE__ //这是一个特殊的预定义宏，用于获取代码编译日期的字符串。
#define KERNEL_TIME		__TIME__//这是一个特殊的预定义宏，用于获取代码编译时间的字符串。
#define KERNEL_LICENCE	"Apache License"	/* license */
#define KERNEL_COMPUTERNAME	"test-pc"	/* default name for the machine */


/* identifiant du processeur */
#ifdef __x86__
#define KERNEL_PROCESSOR_IDENTIFIER "x86"
#else
#define KERNEL_PROCESSOR_IDENTIFIER "(null)"
#endif


/* max open file */
#define CONFIG_MAX_FILE	32

#endif