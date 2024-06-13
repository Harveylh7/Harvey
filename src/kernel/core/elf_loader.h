#ifndef ELF_H
#define ELF_H

#include <runtime/types.h>
#include <process.h>


typedef struct
{
    unsigned char e_ident[16]; // 包含 ELF 标识信息,如 ELF 版本、目标架构（32 位或 64 位）和字节序。
    u16 e_type;                // 指定 ELF 文件的类型
    u16 e_machine;             // 指示文件所针对的架构
    u32 e_version;             // ELF 格式的版本
    u32 e_entry;               // 进程开始执行的内存地址。即程序的入口点
    u32 e_phoff;               // 程序头表在文件中的偏移。程序头表包含有关程序段的信息
    u32 e_shoff;               // 节头表在文件中的偏移
    u32 e_flags;               // 保存与文件相关的处理器特定标志。
    u16 e_ehsize;              // 指定 ELF 头本身的大小。对于 32 位 ELF 头，通常为 52 字节。

    u16 e_phentsize; // 这是程序头表中每个条目的大小
    u16 e_phnum;     // 这个字段表示程序头表中的条目数。

    u16 e_shentsize; // 这是节头表中每个条目的大小。
    u16 e_shnum;     // 这个字段表示节头表中的条目数。

    u16 e_shstrndx; // 这个字段指定包含节名称的节头表条目的索引。用于定位节名称字符串表。
} Elf32_Ehdr;

// 这些宏主要用来识别和解释 e_ident 数组中的特定字节
#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_PAD 7

// 宏定义了 ELF 文件的魔术数字，前四个字节用于标识文件为 ELF 文件：
// 这四个字节组成了字符串 0x7f 'E' 'L' 'F'，即 "ELF" 的 ASCII 表示。
// 这个特定的字节序列被称为 ELF 文件的魔术数字（Magic Number）。如果一个文件的前四个字节匹配这个序列，则可以确定该文件是一个 ELF 文件。这是文件格式识别的一种标准方法，确保只有符合该格式的文件才能被正确地解析和处理。
#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

/* EI_CLASS */
#define ELFCLASSNONE 0 /* invalid class */
#define ELFCLASS32 1   /* 32-bit objects */
#define ELFCLASS64 2   /* 64-bit objects */

// ELF 文件的类（也即 ELF 文件的位数）
#define ELFCLASSNONE 0 // 无效的类。
#define ELFCLASS32 1   // 32 位对象。
#define ELFCLASS64 2   // 64 位对象。

// 这些宏定义了 ELF 文件头部中 e_ident 数组的第五个字节（即 e_ident[EI_DATA]）的可能值，用于指示文件数据的字节序（即数据的字节编码顺序）。
// 字节序决定了多字节数据（例如整数）在内存中的排列方式。
#define ELFDATANONE 0 // 无效的数据编码。
#define ELFDATA2LSB 1 // 小端序（Least Significant Byte first）。
#define ELFDATA2MSB 2 // 大端序（Most Significant Byte first）。

/* EI_VERSION 指示 ELF 文件头的版本*/
#define EV_CURRENT 1          // 当前版本
#define ELFVERSION EV_CURRENT // 当前版本的别名

// 结构体表示 ELF（可执行与可链接格式）文件中的程序头（Program Header）
typedef struct
{
    u32 p_type;   // 段的类型。
    u32 p_offset; // 段在文件中的偏移。
    u32 p_vaddr;  // 段在文件中的偏移。
    u32 p_paddr;  // 段在内存中的物理地址（如果适用）。
    u32 p_filesz; // 段在文件中的大小。
    u32 p_memsz;  // 段在内存中的大小。
    u32 p_flags;  // 与段相关的标志。
    u32 p_align;  // 段的对齐要求。
} Elf32_Phdr;

//
#define PT_NULL 0            // 忽略该段
#define PT_LOAD 1            // 可加载段
#define PT_DYNAMIC 2         // 动态链接
#define PT_INTERP 3          // 解释器路径名。
#define PT_NOTE 4            // 附加信息。
#define PT_SHLIB 5           // 保留（未使用）。
#define PT_PHDR 6            // 程序头表自身。
#define PT_LOPROC 0x70000000 // 处理器特定的最低类型值。
#define PT_HIPROC 0x7fffffff // 处理器特定的最高类型值。

// p_flags 字段的可能值，用于指示程序段（segment）的权限。
/* p_flags */
#define PF_X 0x1 // 可执行段。
#define PF_W 0x2 // 可写段。
#define PF_R 0x4 // 可读段。

// 定义了 ELF 文件中节（section）的类型（type）
// 在 ELF 文件中，"节"（section）是指文件的一部分，用于组织和存储特定类型的数据或信息。
// 每个节都有一个特定的类型（如代码、数据、符号表等），并且具有自己的属性和内容。
enum eElfSectionTypes
{
    SHT_NULL,                // 0，表示无效的节类型
    SHT_PROGBITS,            // 1，表示包含程序数据的节
    SHT_SYMTAB,              // 2，表示符号表节
    SHT_STRTAB,              // 3，表示字符串表节
    SHT_RELA,                // 4，表示重定位入口表，带有显式加法偏移量
    SHT_HASH,                // 5，表示符号哈希表节
    SHT_DYNAMIC,             // 6，表示动态链接信息节
    SHT_NOTE,                // 7，表示附加信息节
    SHT_NOBITS,              // 8，表示不占用文件空间的节
    SHT_REL,                 // 9，表示重定位入口表，不带加法偏移量
    SHT_SHLIB,               // A，保留但未指定用途的节
    SHT_DYNSYM,              // B，表示动态链接符号表节
    SHT_LAST,                // C，表示最后一个有效节类型
    SHT_LOPROC = 0x70000000, // 处理器特定的最低节类型值
    SHT_HIPROC = 0x7fffffff, // 处理器特定的最高节类型值
    SHT_LOUSER = 0x80000000, // 用户特定的最低节类型值
    SHT_HIUSER = 0xffffffff  // 用户特定的最高节类型值
};

// ELF 文件中的节头（Section Header)
typedef struct
{
    u32 name;      // 节的名称在字符串表中的偏移量。
    u32 type;      // 节的类型，表示节所包含的数据的类型或者节的属性。
    u32 flags;     // 节的标志，描述节的属性或者访问权限。
    u32 address;   // 在内存中的地址（如果适用），表示节在内存中的起始地址。
    u32 offset;    // 在文件中的偏移量，表示节在 ELF 文件中的位置。
    u32 size;      // 节的大小，表示节所占用的字节数。
    u32 link;      // 链接字段，用于指示其他节的索引。
    u32 info;      // 其他信息字段，用于特定类型的节存储额外的信息。
    u32 addralign; // 地址对齐，表示节在内存中的地址的对齐方式。
    u32 entsize;   // 项大小，用于指定节中每个条目的大小，仅适用于某些类型的节。
} Elf32_Shdr;

enum
{
    R_386_NONE = 0, // 表示不需要进行重定位。
    R_386_32,       // 对应的重定位公式为 S + A，其中 S 为符号值，A 为相对偏移量
    R_386_PC32,     // 对应的重定位公式为 S + A - P，其中 S 为符号值，A 为相对偏移量，P 为指令的地址。
    R_386_GOT32,    // 对应的重定位公式为 G + A - P，其中 G 为全局偏移表（Global Offset Table）中的条目，A 为相对偏移量，P 为指令的地址。
    R_386_PLT32,    // 对应的重定位公式为 L + A - P，其中 L 为过程链接表（Procedure Linkage Table）中的条目，A 为相对偏移量，P 为指令的地址。
    R_386_COPY,     // 表示不需要进行重定位。
    R_386_GLOB_DAT, // 对应的重定位公式为 S，其中 S 为符号值。
    R_386_JMP_SLOT, // 对应的重定位公式为 S，其中 S 为符号值。
    R_386_RELATIVE, // 对应的重定位公式为 B + A，其中 B 为基地址（Base Address），A 为相对偏移量。
    R_386_GOTOFF,   // 对应的重定位公式为 S + A - GOT，其中 S 为符号值，A 为相对偏移量，GOT 为全局偏移表的基地址。
    R_386_GOTPC,    // 对应的重定位公式为 GOT + A - P，其中 GOT 为全局偏移表的基地址，A 为相对偏移量，P 为指令的地址。
    R_386_LAST      // 表示不需要进行重定位。
};


//表示 ELF 文件中动态链接信息表（Dynamic Section）中的每个条目
typedef struct{
    u16 d_tag;
    u32 d_val;  //也可以理解为指针（Pointer）的值，具体用途取决于标签的含义。
}Elf32_dyn;

enum {
	DT_NULL,	//表示动态链接信息列表的结束
	DT_NEEDED,	//指示依赖的共享库名称在字符串表中的偏移量。
	DT_PLTRELSZ,//表示 PLT（Procedure Linkage Table）的大小（以字节为单位）。
	DT_PLTGOT,	//指示 PLT/GOT（Global Offset Table）的地址。
	DT_HASH,	//指示符号哈希表的地址。
	DT_STRTAB,	//指示字符串表的地址。
	DT_SYMTAB,	//指示符号表的地址。
	DT_RELA,	//指示重定位表的地址，使用 RELA 格式的重定位条目。
	DT_RELASZ,	//指示 RELA 格式的重定位表的大小（以字节为单位）。
	DT_RELAENT,	//指示 RELA 格式的重定位表中每个条目的大小（以字节为单位）。
	DT_STRSZ,	//指示字符串表的大小（以字节为单位）。
	DT_SYMENT,	//指示符号表中每个条目的大小（以字节为单位）。
	DT_INIT,	//指示初始化函数的地址。
	DT_FINI,	//指示终止函数的地址。
	DT_SONAME,	//指示共享对象名称在字符串表中的偏移量。
	DT_RPATH,	//指示库路径在字符串表中的偏移量。
	DT_SYMBOLIC,//指示对于库的符号搜索顺序是反向的，先搜索库再搜索可执行文件。
	DT_REL,		//指示重定位表的地址，使用 REL 格式的重定位条目。
	DT_RELSZ,	//指示 REL 格式的重定位表的大小（以字节为单位）。
	DT_RELENT,	//指示 REL 格式的重定位表中每个条目的大小（以字节为单位）。
	DT_PLTREL,	//指示 PLT 的重定位格式。
	DT_DEBUG,	//指示调试信息的入口，内容未知。
	DT_TEXTREL,	//指示可能对非可写段进行修改。
	DT_JMPREL,	//指示 PLT 的重定位表的地址。
	DT_LOPROC = 0x70000000,	//用于处理器特定的低端范围。
	DT_HIPROC = 0x7FFFFFFF	//用于处理器特定的高端范围。
};




int is_elf(char *);//取文件头并检查文件的魔术数字来确定文件类型是否为 ELF。
u32 load_elf(char *,process_st *);//加载给定的 ELF 格式可执行文件，并将其映射到内存中，然后将进程的相关信息存储到 process_st 结构体中。

int execv(char* file,int argc,char** argv);//执行指定的可执行文件，并传递给它一组参数。
void execv_module(u32 entry,int argc,char** argv);//用于执行指定的模块，并传递给它一组参数。




#endif