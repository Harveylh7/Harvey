#ifndef IO_H
#define IO_H

#include <runtime/types.h>

// 这些地址是固定的，这是由于IBM PC兼容系统的设计标准所决定的。在文本模式下，视频内存的起始地址和大小是硬编码在PC硬件和操作系统中的。

// 以下是详细说明：
// 1. **RAMSCREEN 0xB8000**：
//    - 这是文本模式下视频内存的起始地址。对于大多数PC兼容系统，当视频卡处于80x25字符文本模式（VGA文本模式）时，视频内存从地址 `0xB8000` 开始。
//    - 每个字符占用2个字节：一个字节用于ASCII码，一个字节用于属性（如颜色）。
//    - 这个地址是固定的，标准的IBM PC兼容系统都使用这个地址来存储文本模式下的显示内容。

// 2. **SIZESCREEN 0xFA0**：
//    - 这是文本模式下视频内存的大小。`0xFA0` 相当于4000字节，即2000个字符。
//    - 在80列x25行的文本模式下，屏幕可以显示2000个字符，每个字符2个字节，共4000字节。
//    - 这个大小是基于标准的80x25字符模式计算得来的。

// 3. **SCREENLIM 0xB8FA0**：
//    - 这是视频内存的结束地址。它是视频内存起始地址加上视频内存大小。
//    - `0xB8000 + 0xFA0 = 0xB8FA0`，表示文本模式下视频内存的最后一个字节的地址。
#define RAMSCREEN 0xB8000 // 视屏内存的起始地址
#define SIZESCREEN 0xFA0  // 文本模式下视屏所占用的内存大小
#define SCREENLIM 0xB8FA0 // 视屏内存的结束地址

// 输入输出类

class Io
{
public:
    Io();
    Io(u32 flag);
    enum Colour
    {
        Black = 0,
        Blue = 1,
        Green = 2,
        Cyan = 3,
        Red = 4,
        Magenta = 5,
        Orange = 6,
        LightGrey = 7,
        DarkGrey = 8,
        LightBlue = 9,
        LightGreen = 10,
        LightCyan = 11,
        LightRed = 12,
        LightMagenta = 13,
        Yellow = 14,
        White = 15
    };
    void outb(u32 ad, u8 v);  // 向指定的I/O端口写入一个字节
    void outw(u32 ad, u16 v); // 向指定的I/O端口写入一个字（两个字节）
    void outl(u32 ad, u32 v); // 向指定的I/O端口写入一个双字（四个字节）

    u8 inb(u32 ad);  // 从指定的I/O读取一个字节
    u16 inw(u32 ad); // 从指定的I/O读取一个字(两字节)
    u32 inl(u32 ad); // 从指定的I/O读取一个双字（四字节）

    void putctty(char c);                // 在控制台上输出一个字节
    u32 read(char *buf, u32 count);      // 从控制台读取一个字符串
    void putc(char c);                   // 在屏幕上输出一个字节
    void setColor(char fcol, char bcol); // 改变颜色
    void setXY(char xc, char yc);        // 改变光标位置
    void clear();                        // 清屏
    void print(const char *s, ...);      // 在屏幕上输出一个字符串

    u32 getX();
    u32 getY();

    void switchtty(); // 更改I/O接口

    /** x86 functions **/
    void scrollup(unsigned int n); //滚动屏幕
    void save_screen(); //保存屏幕内容
    void load_screen(); //加载屏幕内容

    enum ConsoleType {
			BUFFERED,
			GETCHAR
	};

    static Io*	current_io;
	static Io*	last_io;

private:

		/** x86 private attributes **/
		char*	real_screen;
		char	screen[SIZESCREEN];
		
		char	inbuf[512];		/* console buffer */
		int		keypos;			/* console read position */
		int		inlock;			/* console state */
		int		keystate;		/* console type keyboard */
		
		
		char 	fcolor;			/* console foreground color */
		char	bcolor;			/* console background color */
		char	x;				/* console x position */
		char	y;				/* console y position */
		char kattr;				/* console attribut */
		static char*	vidmem;	/* screen video memory */
};

/** standart starting io interface **/
extern Io io;

#endif