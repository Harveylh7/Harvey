#ifndef __MY_BOOT__
#define __MY_BOOT__

#include <runtime/types.h>

// 存储 Multiboot 信息的结构体。用于引导多个操作系统或内核的标准
// Multiboot标准主要包括以下内容：
// Multiboot引导加载程序（bootloader）： Multiboot引导加载程序是符合Multiboot标准的引导程序，它能够读取Multiboot兼容的操作系统内核，并将控制权交给这些内核，从而启动相应的操作系统。
// Multiboot操作系统内核： Multiboot操作系统内核是符合Multiboot标准的操作系统内核，它能够被Multiboot引导加载程序所识别和加载。
// Multiboot信息结构： Multiboot标准定义了一个用于传递信息的数据结构，称为Multiboot信息结构。引导加载程序通过这个结构向操作系统内核传递各种信息，如内存布局、命令行参数、引导设备等。

struct multiboot_info
{
    u32 flags;       // 标志字段，指示结构体中哪些字段有效
    u32 low_mem;     // 低端内存大小
    u32 high_mem;    // 高端内存大小
    u32 boot_device; // 引导设备
    u32 cmdline;     // 内核命令行地址
    u32 mods_count;  // 模块数量
    u32 mods_addr;   // 模块地址
    struct
    {
        u32 num;
        u32 size;
        u32 addr;
        u32 shndx;
    } elf_sec;                      // ELF section 信息
    
    unsigned long mmap_length;      // 内存映射表的长度
    unsigned long mmap_addr;        // 内存映射表的地址
    unsigned long drives_length;    // 驱动器列表的长度
    unsigned long drives_addr;      // 驱动器列表的地址
    unsigned long config_table;     // 配置表的地址
    unsigned long boot_loader_name; // 引导加载程序的名字
    unsigned long apm_table;        // APM 表的地址
    unsigned long vbe_control_info; // VBE 控制信息的地址
    unsigned long vbe_mode_info;    // VBE 模式信息的地址
    unsigned long vbe_mode;         // VBE 模式
    unsigned long vbe_interface_seg;
    unsigned long vbe_interface_off;
    unsigned long vbe_interface_len;
};

// VBE 控制器信息
struct vbe_controller
{
    unsigned char signature[4];      // 签名
    unsigned short version;          // 版本
    unsigned long oem_string;        // OEM 字符串地址
    unsigned long capabilities;      // 功能
    unsigned long video_mode;        // 视频模式
    unsigned short total_memory;     // 总内存大小
    unsigned short oem_software_rev; // OEM 软件版本
    unsigned long oem_vendor_name;   // OEM 厂商名称地址
    unsigned long oem_product_name;  // OEM 产品名称地址
    unsigned long oem_product_rev;   // OEM 产品版本地址
    unsigned char reserved[222];     // 保留字节
    unsigned char oem_data[256];     // OEM 数据
} __attribute__((packed));           // 确保内存布局紧凑

// VBE 模式信息
//用于提供对图形模式和图形功能的扩展支持。
//它允许操作系统和应用程序直接访问图形硬件，从而实现更高级的图形显示和操作。
struct vbe_mode
{
    unsigned short mode_attributes;    // 模式属性
    unsigned char win_a_attributes;    // 窗口 A 的属性
    unsigned char win_b_attributes;    // 窗口 B 的属性
    unsigned short win_granularity;    // 窗口粒度
    unsigned short win_size;           // 窗口大小
    unsigned short win_a_segment;      // 窗口 A 段
    unsigned short win_b_segment;      // 窗口 B 段
    unsigned long win_func;            // 窗口功能
    unsigned short bytes_per_scanline; // 每行字节数

    // >=1.2
    unsigned short x_resolution;         // X 分辨率
    unsigned short y_resolution;         // Y 分辨率
    unsigned char x_char_size;           // 水平字符尺寸
    unsigned char y_char_size;           // 垂直字符尺寸
    unsigned char number_of_planes;      // 平面数量
    unsigned char bits_per_pixel;        // 每像素位数
    unsigned char number_of_banks;       // 银行数量
    unsigned char memory_model;          // 内存模式
    unsigned char bank_size;             // 银行大小
    unsigned char number_of_image_pages; // 图像页数量
    unsigned char reserved0;

    // 直接色彩
    unsigned char red_mask_size;           // 红色掩码位数
    unsigned char red_field_position;      // 红色字段位置
    unsigned char green_mask_size;         // 绿色掩码位数
    unsigned char green_field_position;    // 绿色字段位置
    unsigned char blue_mask_size;          // 蓝色掩码位数
    unsigned char blue_field_position;     // 蓝色字段位置
    unsigned char reserved_mask_size;      // 保留掩码位数
    unsigned char reserved_field_position; // 保留字段位置
    unsigned char direct_color_mode_info;

    // >=2.0
    unsigned long phys_base;
    unsigned long reserved1;
    unsigned short reversed2;

    // >=3.0
    unsigned short linear_bytes_per_scanline;
    unsigned char banked_number_of_image_pages;
    unsigned char linear_number_of_image_pages;
    unsigned char linear_red_mask_size;
    unsigned char linear_red_field_position;
    unsigned char linear_green_mask_size;
    unsigned char linear_green_field_position;
    unsigned char linear_blue_mask_size;
    unsigned char linear_blue_field_position;
    unsigned char linear_reserved_mask_size;
    unsigned char linear_reserved_field_position;
    unsigned long max_pixel_clock;

    unsigned char reserved3[189];
} __attribute__((packed));

#endif
