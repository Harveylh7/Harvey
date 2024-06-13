#include <core/os.h>

Io* Io::last_io=&io;
Io* Io::current_io=&io;

char* Io::vidmem = (char*)RAMSCREEN;//初始化视屏内存的起始地址


Io::Io(){
    real_screen = (char*)RAMSCREEN;
}

Io::Io(u32 flag){
    real_screen=(char*)screen;
}



//__asm__ __volatile__ ("assembly code" : output operands : input operands : clobbered registers);

//1. "assembly code"
//这是要插入的汇编指令，可以包含占位符，这些占位符用 %0, %1, 等表示，表示操作数的顺序。
//2. output operands
//输出操作数，描述如何将汇编指令的结果传递回 C 变量
//3. input operands
//输入操作数，描述从 C 变量传递到汇编指令的输入。
//4. clobbered registers
//列出在汇编代码中可能被修改的寄存器，通知编译器这些寄存器在执行汇编代码后需要被重新加载。


/* 向指定的 I/O 端口地址写入一个字节数据，从而实现数据输出*/  //不是很懂
void Io::outb(u32 ad, u8 v) {
    // 使用内联汇编将值 v 输出到 I/O 端口地址 ad
    asmv("outb %%al, %%dx" :: "d" (ad), "a" (v));//这段代码将 C 变量 ad 和 v 分别加载到 dx 和 al 寄存器中，然后执行 outb 指令，将 al 寄存器中的值（即 v）发送到 dx 寄存器指定的 I/O 端口（即 ad）。
}


/* output word */
void Io::outw(u32 ad,u16 v){
	asmv("outw %%ax, %%dx" :: "d" (ad), "a" (v));
}
/* output word */
void Io::outl(u32 ad,u32 v){
	asmv("outl %%eax, %%dx" : : "d" (ad), "a" (v));
}
/* input byte */
u8 Io::inb(u32 ad){
	u8 _v;       \
	asmv("inb %%dx, %%al" : "=a" (_v) : "d" (ad)); \
	return _v;
}
/* input word */
u16	Io::inw(u32 ad){
	u16 _v;			\
	asmv("inw %%dx, %%ax" : "=a" (_v) : "d" (ad));	\
	return _v;
}
/* input word */
u32	Io::inl(u32 ad){
	u32 _v;			\
	asmv("inl %%dx, %%eax" : "=a" (_v) : "d" (ad));	\
	return _v;
}

/* renvoie la position x du curseur */
u32	Io::getX(){
	return (u32)x;
}

/* renvoie la position y du curseur */
u32	Io::getY(){
	return (u32)y;
}





/* x86 scroll up screen */
void Io::scrollup(unsigned int n)
{
		unsigned char *video, *tmp;

		for (video = (unsigned char *) real_screen;
		     video < (unsigned char *) SCREENLIM; video += 2) {//+2是因为字符+属性
			tmp = (unsigned char *) (video + n * 160);//每行有80个字节

			if (tmp < (unsigned char *) SCREENLIM) {
				*video = *tmp;
				*(video + 1) = *(tmp + 1);
			} else {
				*video = 0;
				*(video + 1) = 0x07;
			}
		}

		y -= n;//设置光标到回滚前的位置
		if (y < 0)
			y = 0;
}


/* sauvegarde la memoire video */
void Io::save_screen(){
	memcpy(screen,(char*)RAMSCREEN,SIZESCREEN);
	real_screen=(char*)screen;
}

/* charge la memoire video */
void Io::load_screen(){
	memcpy((char*)RAMSCREEN,screen,SIZESCREEN);
	real_screen=(char*)RAMSCREEN;
}


/* switch tty io */
void Io::switchtty(){

	char tmp[SIZESCREEN];
    memcpy(tmp,(char*)RAMSCREEN,SIZESCREEN);
	load_screen();
    memcpy(screen,tmp,SIZESCREEN);
	
    last_io=current_io;
	current_io=this;
}


/* put a byte on screen */
void Io::putc(char c){
	kattr = 0x07;//属性
	unsigned char *video;
	video = (unsigned char *) (real_screen+ 2 * x + 160 * y);
	if (c == 10) {		//ascii为10是换行字符	
		x = 0;
		y++;
	} else if (c == 8) {	//8为退格字符
		if (x) {
				*(video + 1) = 0x0;
			x--;
		}
	} else if (c == 9) {	//9为制表符 （TAB键）用于对齐
		x = x + 8 - (x % 8);
	} else if (c == 13) {	//	回车键，光标移到开头
		x = 0;
	} else {		
			*video = c;
			*(video + 1) = kattr;

		x++;
		if (x > 79) {//超过了一行，则进入下一行的开头
			x = 0;
			y++;
		}
	}
	//一面屏幕存储4000字节，一行有160字节，80个字，一共25行


	if (y > 24)//超过屏幕则进行屏幕滚动
		scrollup(y - 24);
}

/* change colors */
void Io::setColor(char fcol,char bcol){
	fcolor=fcol;
	bcolor=bcol;
}

/* change cursor position */
void Io::setXY(char xc,char yc){
	x=xc;
	y=yc;
}

/* clear screen */
void Io::clear(){
	x=0;
	y=0;
	memset((char*)RAMSCREEN,0,SIZESCREEN);//清空屏幕的内存
}



/* put a string in screen */
void Io::print(const char *s, ...){
	va_list ap;

	char buf[16];
	int i, j, size, buflen, neg;

	unsigned char c;
	int ival;
	unsigned int uival;

	va_start(ap, s);

	while ((c = *s++)) {
		size = 0;
		neg = 0;

		if (c == 0)
			break;
		else if (c == '%') {
			c = *s++;
			if (c >= '0' && c <= '9') {
				size = c - '0';
				c = *s++;
			}

			if (c == 'd') {
				ival = va_arg(ap, int);
				if (ival < 0) {
					uival = 0 - ival;
					neg++;
				} else
					uival = ival;
				itoa(buf, uival, 10);

				buflen = strlen(buf);
				if (buflen < size)
					for (i = size, j = buflen; i >= 0;
					     i--, j--)
						buf[i] =
						    (j >=
						     0) ? buf[j] : '0';

				if (neg)
					print("-%s", buf);
				else
					print(buf);
			}
			 else if (c == 'u') {
				uival = va_arg(ap, int);
				itoa(buf, uival, 10);

				buflen = strlen(buf);
				if (buflen < size)
					for (i = size, j = buflen; i >= 0;
					     i--, j--)
						buf[i] =
						    (j >=
						     0) ? buf[j] : '0';

				print(buf);
			} else if (c == 'x' || c == 'X') {
				uival = va_arg(ap, int);
				itoa(buf, uival, 16);

				buflen = strlen(buf);
				if (buflen < size)
					for (i = size, j = buflen; i >= 0;
					     i--, j--)
						buf[i] =
						    (j >=
						     0) ? buf[j] : '0';

				print("0x%s", buf);
			} else if (c == 'p') {
				uival = va_arg(ap, int);
				itoa(buf, uival, 16);
				size = 8;

				buflen = strlen(buf);
				if (buflen < size)
					for (i = size, j = buflen; i >= 0;
					     i--, j--)
						buf[i] =
						    (j >=
						     0) ? buf[j] : '0';

				print("0x%s", buf);
			} else if (c == 's') {
				print((char *) va_arg(ap, int));//有疑问
				//print(va_arg(ap, char *));
			} 
		} else
			putc(c);
	}

	return;
}

/* put a byte on the console */
void Io::putctty(char c){//放入缓冲区
	if (keystate==BUFFERED){
		if (c == 8) {		/* backspace */
			if (keypos>0) {
				inbuf[keypos--] = 0;
			}
		}
		else if (c == 10) {	/* newline */
			inbuf[keypos++] = c;
			inbuf[keypos] = 0; 
			inlock = 0;
			keypos = 0;
		}
		else {
			inbuf[keypos++] = c; 
		}
	}
	else if (keystate==GETCHAR){
		inbuf[0]=c;
		inbuf[1]=0;
		inlock = 0;
		keypos = 0;
	}
}

/* read a string in the console */
u32 Io::read(char* buf,u32 count){
	if (count>1){
		keystate=BUFFERED;
	}
	else{	//getchar
		keystate=GETCHAR;
	}
	asm("sti");
	inlock=1;
	while (inlock == 1);
	asm("cli");
	strncpy(buf,inbuf,count);
	return strlen(buf);
}
























