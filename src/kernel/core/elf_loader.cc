#include <core/os.h>


//，在加载器中，如果没有为某个处理函数指定特定名称，系统将生成一个默认名称 _proc_，然后加上一个数字后缀，
//从 0 开始递增。例如，第一个默认处理函数名称可能是 _proc_0，第二个是 _proc_1，依此类推


char* __default_proc_name="_proc_";	/* nom par default avec en plus un nombre */
char 	nb_default='0';



//测试传入的文件是否为 ELF 格式。
int is_elf(char *file)
{
	Elf32_Ehdr *hdr;

	hdr = (Elf32_Ehdr *) file;//强转
	if (hdr->e_ident[0] == ELFMAG0 && hdr->e_ident[1] == ELFMAG1
	    && hdr->e_ident[2] == ELFMAG2 && hdr->e_ident[3] == ELFMAG3)
		return RETURN_OK;
	else
		return ERROR_PARAM;
}

//这段代码的主要功能是将 ELF 文件中的可加载段复制到虚拟内存中，并正确设置进程结构体中的相关字段，最后返回程序的入口地址。
/*
 *	将 ELF 文件加载到虚拟内存中，并返回程序的入口地址
 */
u32 load_elf(char *file, process_st *proc)
{
	char *p; // 临时指针用于初始化未初始化的内存
	u32 v_begin, v_end; // 段的起始和结束虚拟地址
	Elf32_Ehdr *hdr; // ELF 文件头部指针
	Elf32_Phdr *p_entry; // 程序头表指针
	Elf32_Shdr *s_entry; // 节头表指针
	int i, pe; // 循环变量

	// 将文件内容解释为 ELF 文件头部
	hdr = (Elf32_Ehdr *) file;
	// 将文件内容解释为程序头表，偏移量由文件头提供
	p_entry = (Elf32_Phdr *) (file + hdr->e_phoff);
	// 将文件内容解释为节头表，偏移量由文件头提供
	s_entry = (Elf32_Shdr *) (file + hdr->e_shoff);
	
	// 检查文件是否为 ELF 格式
	if (is_elf(file) == ERROR_PARAM) {
		io.print("INFO: load_elf(): file not in ELF format !\n");
		return 0; // 如果不是 ELF 格式，返回 0 表示失败
	}
	
	// 遍历程序头表中的每个条目
	for (pe = 0; pe < hdr->e_phnum; pe++, p_entry++) {

		// 只处理类型为 PT_LOAD 的段
		if (p_entry->p_type == PT_LOAD) {
			// 获取段的虚拟地址范围
			v_begin = p_entry->p_vaddr;
			v_end = p_entry->p_vaddr + p_entry->p_memsz;

			// 检查段的起始地址是否低于用户空间的最小地址
			if (v_begin < USER_OFFSET) {
				io.print("INFO: load_elf(): can't load executable below %p\n", USER_OFFSET);
				return 0; // 如果低于，返回 0 表示失败
			}

			// 检查段的结束地址是否高于用户空间的最大地址
			if (v_end > USER_STACK) {
				io.print("INFO: load_elf(): can't load executable above %p\n", USER_STACK);
				return 0; // 如果高于，返回 0 表示失败
			}

			// 描述执行区段和只读数据区段
			if (p_entry->p_flags == (PF_X + PF_R)) {
				proc->b_exec = (char*) v_begin;
				proc->e_exec = (char*) v_end;
			}

			// 描述 BSS 区段
			if (p_entry->p_flags == (PF_W + PF_R)) {
				proc->b_bss = (char*) v_begin;
				proc->e_bss = (char*) v_end;
			}

			// 将段内容从文件复制到虚拟内存
			memcpy((char *) v_begin, (char *) (file + p_entry->p_offset), p_entry->p_filesz);

			// 如果段在内存中的大小大于在文件中的大小，将多余的部分置为 0
			if (p_entry->p_memsz > p_entry->p_filesz)
				for (i = p_entry->p_filesz, p = (char *) p_entry->p_vaddr; i < (int)(p_entry->p_memsz); i++)
					p[i] = 0;		
		}
	}
	// 返回程序入口地址
	return hdr->e_entry;
}
