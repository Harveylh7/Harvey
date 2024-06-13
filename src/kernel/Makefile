ARCH=x86
KERNEL=kernel.elf
SDKDIR=../sdk
INCDIR= -I ./ -I ./modules -I ./core -I ./arch/$(ARCH)


include ./arch/$(ARCH)/config.make

include ./runtime/Makefile
include ./core/Makefile
include ./modules/Makefile
include ./arch/$(ARCH)/Makefile

FLAG :=$(FLAG) -D__$(ARCH)__
PLATFORMS= `find ./arch/ -type d | sed "s/.*\///" | sort`


all: $(KERNEL)

$(KERNEL): $(OBJS)
	$(LD) $(LDFLAG) -o $@ $^ 
	cp $(KERNEL) $(SDKDIR)/bootdisk/
	
help:
	@echo "Makefile for Kernel."
	@echo "Please see COPYING for licensing information."
	@echo "Output should be: "$(KERNEL)
	@echo "Usage: make [ all | clean] " 
	@echo "Currently supported platforms:"
	@echo $(PLATFORMS)
	@echo

tosdk:
	cp $(KERNEL) $(SDKDIR)/disk/

install:
	sudo cp $(KERNEL) /boot/
	
debug:
	$(NM) -n $(KERNEL)

	
hinfo:
	$(OBJDUMP) -f $(KERNEL)
	
dasm:
	$(OBJDUMP) -d $(KERNEL) > dasm.txt
	
				
run:
	cd $(SDKDIR) &&	sh ./diskimage.sh
	cd $(SDKDIR) &&	sh ./qemu.sh
	
geniso:
	cd $(SDKDIR) &&	sh ./cdrom.sh

%.o: %.cc
	$(SC) $(FLAG) -c $< -o  $@
	
%.o: %.S
	$(SC) $(FLAG) -c $< -o  $@
	
%.o: %.asm
	$(ASM) $(ASMFLAG)  -c $< -o  $@


clean:
	rm -f $(OBJS)  $(KERNEL) dasm.txt
	
	


# 这段代码是一个完整的 Makefile，用于构建一个操作系统内核或类似项目。下面是对每个部分的详细解释：

# ### 变量定义

# ```makefile
# ARCH=x86
# KERNEL=kernel.elf
# SDKDIR=../sdk
# INCDIR= -I ./ -I ./modules -I ./core -I ./arch/$(ARCH)
# ```
# - **`ARCH`**: 目标架构，这里是 `x86`。
# - **`KERNEL`**: 要生成的内核文件名，这里是 `kernel.elf`。
# - **`SDKDIR`**: SDK 目录，相对路径为 `../sdk`。
# - **`INCDIR`**: 包含目录选项，用于指定编译器查找头文件的位置。

# ### 包含子 Makefile

# ```makefile
# include ./arch/$(ARCH)/config.make

# include ./runtime/Makefile
# include ./core/Makefile
# include ./modules/Makefile
# include ./arch/$(ARCH)/Makefile
# ```
# - **`include`** 语句用于包含其他 Makefile 文件，将它们的内容插入到当前 Makefile 中。这样可以将构建过程分模块管理，方便维护和组织。

# ### 编译标志和平台

# ```makefile
# FLAG :=$(FLAG) -D__$(ARCH)__
# PLATFORMS= `find ./arch/ -type d | sed "s/.*\///" | sort`
# ```
# - **`FLAG`**: 编译标志，这里添加了一个宏定义 `-D__$(ARCH)__`，用于标识目标架构。
# - **`PLATFORMS`**: 获取所有支持的平台目录，并按字母顺序排序。

# ### 目标规则

# **默认目标：**

# ```makefile
# all: $(KERNEL)
# ```
# - 默认目标是 `all`，它依赖于 `$(KERNEL)`。

# **生成内核：**

# ```makefile
# $(KERNEL): $(OBJS)
# 	$(LD) $(LDFLAG) -o $@ $^ 
# 	cp $(KERNEL) $(SDKDIR)/bootdisk/
# ```
# - 生成内核 `$(KERNEL)` 依赖于所有目标文件 `$(OBJS)`。
# - 使用链接器 `$(LD)` 将目标文件链接成内核文件。
# - 然后将生成的内核文件复制到 `$(SDKDIR)/bootdisk/` 目录。

# ### 其他目标

# **帮助信息：**

# ```makefile
# help:
# 	@echo "Makefile for Kernel."
# 	@echo "Please see COPYING for licensing information."
# 	@echo "Output should be: "$(KERNEL)
# 	@echo "Usage: make [ all | clean] " 
# 	@echo "Currently supported platforms:"
# 	@echo $(PLATFORMS)
# 	@echo
# ```
# - 提供帮助信息，列出当前支持的平台以及如何使用 Makefile。

# **复制内核到 SDK 目录：**

# ```makefile
# tosdk:
# 	cp $(KERNEL) $(SDKDIR)/disk/
# ```
# - 将内核文件复制到 SDK 目录的 `disk` 子目录。

# **安装内核：**

# ```makefile
# install:
# 	sudo cp $(KERNEL) /boot/
# ```
# - 使用 `sudo` 命令将内核文件复制到 `/boot/` 目录。

# **调试和信息：**

# ```makefile
# debug:
# 	$(NM) -n $(KERNEL)

# hinfo:
# 	$(OBJDUMP) -f $(KERNEL)

# dasm:
# 	$(OBJDUMP) -d $(KERNEL) > dasm.txt
# ```
# - **`debug`**: 使用 `$(NM)` 工具列出内核的符号表。
# - **`hinfo`**: 使用 `$(OBJDUMP)` 显示内核文件的文件头信息。
# - **`dasm`**: 使用 `$(OBJDUMP)` 反汇编内核文件，并将结果输出到 `dasm.txt`。

# **运行和生成 ISO 镜像：**

# ```makefile
# run:
# 	cd $(SDKDIR) &&	sh ./diskimage.sh
# 	cd $(SDKDIR) &&	sh ./qemu.sh

# geniso:
# 	cd $(SDKDIR) &&	sh ./cdrom.sh
# ```
# - **`run`**: 在 `$(SDKDIR)` 目录中运行 `diskimage.sh` 和 `qemu.sh` 脚本。
# - **`geniso`**: 在 `$(SDKDIR)` 目录中运行 `cdrom.sh` 脚本。

# ### 模式规则

# **编译 C++ 源文件：**

# ```makefile
# %.o: %.cc
# 	$(SC) $(FLAG) -c $< -o  $@
# ```
# - 将 `.cc` 文件编译为 `.o` 文件，使用 `$(SC)` 编译器和 `$(FLAG)` 编译标志。

# **编译汇编文件：**

# ```makefile
# %.o: %.S
# 	$(SC) $(FLAG) -c $< -o  $@

# %.o: %.asm
# 	$(ASM) $(ASMFLAG)  -c $< -o  $@
# ```
# - 将 `.S` 或 `.asm` 文件编译为 `.o` 文件，分别使用 `$(SC)` 编译器和 `$(ASM)` 汇编器。

# ### 清理目标

# ```makefile
# clean:
# 	rm -f $(OBJS)  $(KERNEL) dasm.txt
# ```
# - 删除所有目标文件、生成的内核文件和 `dasm.txt` 文件，以清理构建环境。

# ### 总结

# 这个 Makefile 组织了构建内核的过程，定义了变量、包含了其他 Makefile、指定了目标和依赖关系，以及提供了清理、安装和调试等辅助目标。通过这种模块化的方式，可以有效管理复杂项目的构建过程。
