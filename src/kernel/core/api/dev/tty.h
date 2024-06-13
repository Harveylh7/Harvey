//TTY是终端设备（Teletype）。
//在Unix和类Unix系统中，TTY是指任何通过串行线路（例如串行端口）进行的终端连接。
//TTY还指代虚拟控制台，通常以文本模式提供，用于在图形用户界面之外进行命令行交互。
//TTY提供了用户与计算机系统进行交互的接口，允许用户输入命令并查看系统的输出。
#ifndef __API_TTY__
#define __API_TTY__

#include <api/dev/keyboard.h>

#define TTY_NAME_LEN	16  // TTY名称最大长度

// TTY信息结构体（静态部分）
struct tty_info_static {
    char name[TTY_NAME_LEN];    // TTY名称
    char state;                 // TTY状态
    char type;                  // TTY类型
    unsigned int flags;         // TTY标志位
};

// TTY信息结构体（移动部分）
struct tty_info_moving {
    unsigned int x;     // X坐标
    unsigned int y;     // Y坐标
    unsigned int attrf; // 前景色
    unsigned int attrb; // 背景色
};

// TTY类型枚举
enum TTY_Type {
    TTY_TYPE_IOSTD = 0,     // 标准输入输出
    TTY_TYPE_SERIAL = 1,    // 串行TTY
    TTY_TYPE_SCREEN = 2,    // 屏幕TTY
    TTY_TYPE_VIRTUAL = 3,   // 虚拟TTY
    TTY_TYPE_GUI = 4        // 图形用户界面TTY
};

// TTY状态枚举
enum TTY_State {
    TTY_STATE_RUN = 0,      // 运行中
    TTY_STATE_SWITCH = 1,   // 切换中
    TTY_STATE_ERROR = 2,    // 错误
    TTY_STATE_PAUSE = 3     // 暂停
};

// TTY颜色枚举
enum TTY_Colour {
    Black = 0,
    Blue,
    Green,
    Cyan,
    Red,
    Magenta,
    Orange,
    LightGrey,
    DarkGrey,
    LightBlue,
    LightGreen,
    LightCyan,
    LightRed,
    LightMagenta,
    Yellow,
    White
};

// TTY相关的ioctl命令
#define API_TTY_SWITCH_SCREEN   0xff52  // 切换TTY屏幕
#define API_TTY_CLEAR_SCREEN    0xff53  // 清空TTY屏幕
#define API_TTY_GET_SINFO       0xff54  // 获取TTY静态信息
#define API_TTY_GET_MINFO       0xff55  // 获取TTY移动信息
#define API_TTY_SET_MINFO       0xff56  // 设置TTY移动信息



//在操作系统的设计中，内核通常会定义一些特殊的系统调用（syscall），这些系统调用允许用户空间的程序向内核发出请求，以执行特定的操作。这些系统调用的唯一标识是其调用号（或称为系统调用号），而该号码是内核中用于标识该调用的一个整数值。
//在这个特定的情况下，API_TTY_SWITCH_SCREEN被定义为0xff52，这意味着在系统调用表中，对应的调用号是0xff52。当用户程序调用该系统调用时，它实际上向内核发出了一个请求，请求切换到另一个屏幕。内核会识别这个请求，并执行相应的操作来实现屏幕切换。


#endif