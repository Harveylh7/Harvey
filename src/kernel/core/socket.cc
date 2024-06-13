#include <core/os.h>

Socket::~Socket(){
    fsm.addFile("/sys/sockets/",this);
}

Socket::Socket(char* n) : File(n,TYPE_FILE)//初始化基类 ,为普通文件
{
    fsm.addFile("/sys/sockets/",this);
}


u32 Socket::open(u32 flag){
    return RETURN_OK;
}


u32	Socket::close(){
	return RETURN_OK;
}

u32	Socket::read(u8* buffer,u32 size){
	return NOT_DEFINED;
}

u32	Socket::write(u8* buffer,u32 size){
	return NOT_DEFINED;
}

u32	Socket::ioctl(u32 id,u8* buffer){
	return NOT_DEFINED;
}

u32	Socket::remove(){
	delete this;
}

void Socket::scan(){

}



