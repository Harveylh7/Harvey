#include <core/os.h>
#include <runtime/buffer.h>

Buffer::Buffer(char* n,u32 siz){
    map = (char*)kmalloc(siz);
    size=siz;
    memcpy(map,n,siz);
}

Buffer::~Buffer(){
    size=0;
    map=NULL;
}


void Buffer::add(u8* c,u32 s){
    char* old=map;
    map=(char*)kmalloc(size + s);
    memcpy(map,old,size);
    kfree(old);
    memcpy((char*)(map+size),(char*)c,s);
    size=size+s;
}

u32 Buffer::get(u8 *c,u32 s){//拿走buffer结尾的s个字符放到c中
    if(s>size)s=size;
    memcpy((char*)c,(char*)(map+(size - s )),s);
    //拿走后buffer进行修改
    char *old = map;
    map = (char*)kmalloc(size-s);
    memcpy(map,old,(size-s));
    kfree(old);
    size = size -s;
    return s;//拿走s个字符
}

u32 Buffer::isEmpty(){
    if(size==0)return 1;
    else return 0;
}

void Buffer::clear(){
    size =0;
    if(map!=NULL)kfree(map);
}
//char data[100];
//buf >> data;//map中的数据复制到c中
Buffer &Buffer::operator>>(char*c)//没做判空
{
    memcpy(c,map,size);
    return *this;
}


