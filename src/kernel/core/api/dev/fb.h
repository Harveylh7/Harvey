//帧缓冲


//帧缓冲（Framebuffer）是计算机系统中用于存储图像数据的内存区域。
//它通常用于显示器输出，作为计算机显示图形图像的一种方式。帧缓冲存储了图像的每个像素的颜色信息，以及可能的其他相关信息，例如像素的位置和透明度等。
//在图形用户界面（GUI）中，帧缓冲扮演着重要的角色。当计算机需要在屏幕上显示图像时，它会将图像数据写入帧缓冲，然后通过显示控制器将帧缓冲的内容传输到显示器上，从而呈现图像。
//这种方式能够实现高效的图形显示，并允许计算机以逐帧方式更新屏幕上的内容，实现流畅的用户体验。




#ifndef __API_FB__
#define __API_FB__

// 结构体定义：表示帧缓冲信息
struct fb_info {
    unsigned int w;    // 帧缓冲的宽度
    unsigned int h;    // 帧缓冲的高度
    char bpp;          // 每个像素的位数（比特数）
    char state;        // 帧缓冲状态（活跃或非活跃）
    unsigned int* vmem;  // 指向帧缓冲的虚拟内存地址
};

// 定义帧缓冲状态的枚举值
enum {
    FB_NOT_ACTIVE = 0,  // 帧缓冲非活跃状态
    FB_ACTIVE = 1       // 帧缓冲活跃状态
};

// 定义帧缓冲相关的系统调用
#define API_FB_IS_AVAILABLE  0x801   // 查询帧缓冲是否可用
#define API_FB_GET_INFO      0x802   // 获取当前帧缓冲信息
#define API_FB_GET_BINFO     0x803   // 获取最佳帧缓冲信息
#define API_FB_SET_INFO      0x804   // 设置帧缓冲信息


#endif  // __API_FB__
