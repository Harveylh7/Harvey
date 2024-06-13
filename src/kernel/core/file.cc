#include <core/os.h>

//替换字符串中特定的字符
static void strreplace(char *s,char a,char to)//a表示要被替换的字符，to表示替换成的字符
{
    if(s==NULL)return;
    while(*s)
    {
        if(*s==a)
        {
            *s = to;
        }
        ++s;
    }
    return;
} 


u32 File::inode_system =0;

//构造函数

File::File(char *n,u8 t)//n为文件名,t为文件类型
{
    name=(char*)kmalloc(strlen(n)+1);
    memset(name,0,strlen(n));
    memcpy(name,n,strlen(n));
    name[strlen(n)]=0;

    checkName();
    master = arch.pcurrent;//所属的主进程的文件对象的指针
    inode = inode_system;
    inode_system++;
    size =0;
    type=t;

    parent=NULL;//初始化时根节点的父节点为NULL
    child =NULL;
    next=NULL;
    prec=NULL;
    link=NULL;
    map_memory=NULL;
}   


File::~File(){
    kfree(name);

    if(prec ==NULL&&next!=NULL)//如果没有前一个兄弟节点（同一个目录下之前的不同文件）
    {
        //将下一个节点设置为子节点
        parent->setChild(next);
        //将新增节点的前一个兄弟节点设置为空
        next->prec =NULL;
    }else if(next==NULL&&prec!=NULL)
    {
        prec->setNext(NULL);
    }else if(next==NULL&&prec==NULL)//说明parent没有孩子了
    {
        parent->setChild(NULL);
    }
    else{
        io.print("prec (%s) next is now %s\n",prec->getName(),next->getName());
		io.print("next (%s) prec is now %s\n",next->getName(),prec->getName());
        //前一个兄弟节点和下一个节点都在，进行双向链表的删除节点
        prec->setNext(next);
        next->setPrec(prec);
    }

    //删除所有子节点
    File* n =child;
    File* nn=NULL;
    
    //删除该节点下所有的子节点
    while(n!=NULL){
        nn=n->getNext();
        delete n;
        n=nn;
    }
}

#define CAR_REPLACE '_'

//替换文件中的特殊字符为CAR_REPLACE
void File::checkName(){
    strreplace(name,' ',CAR_REPLACE);
    strreplace(name,'/',CAR_REPLACE);
	strreplace(name,'\\',CAR_REPLACE);
	strreplace(name,'?',CAR_REPLACE);
	strreplace(name,':',CAR_REPLACE);
	strreplace(name,'>',CAR_REPLACE);
	strreplace(name,'<',CAR_REPLACE);
	strreplace(name,'*',CAR_REPLACE);
	strreplace(name,'"',CAR_REPLACE);
	strreplace(name,':',CAR_REPLACE);
    strreplace(name,'|',CAR_REPLACE);
}

//添加一个子目录
u32 File::addChild(File *n)
{
    if(!n)
    {
        return PARAM_NULL;
    }
    //child为双向链表的头节点，以头插的形式创建节点，child始终为头节点
    n->setParent(this);
    n->setPrec(NULL);
    n->setNext(child);
    if(child!=NULL)
        child->setPrec(n);
    child=n;
    return RETURN_OK;
}


File* File::createChild(char *n,u8 t){
    File* fp=new File(n,t);
    addChild(fp);
    return fp;
}



File*	File::getParent(){
	return parent;
}

File*	File::getChild(){
	return child;
}

File*	File::getNext(){
	return next;
}

File*	File::getPrec(){
	return prec;
}

File*	File::getLink(){
	return link;
}

u32	File::getSize(){
	return size;
}

u32	File::getInode(){
	return inode;
}

void File::scan(){

}

void	File::setType(u8 t){
	type=t;
}

void	File::setSize(u32 t){
	size=t;
}

void	File::setParent(File* n){
	parent=n;
}

void	File::setLink(File* n){
	link=n;
}

void	File::setChild(File* n){
	child=n;
}

void	File::setNext(File* n){
	next=n;
}

void	File::setPrec(File* n){
	prec=n;
}

void File::setName(char*n){
    kfree(name);
    name=(char*)kmalloc(strlen(n)+1);
    memcpy(name,n,strlen(n));
    name[strlen(n)]=0;
    checkName();
}

u8	File::getType(){
	return type;
}

char* File::getName(){
	return name;
}

File* File::find(char* n){
    File* fp =child;
    while(fp!=NULL)
    {
        if(!strcmp(fp->getName(),n))
        {
            return fp;
        }
        fp=fp->next;
    }
    return NULL;
}


u32	File::open(u32 flag){
	return NOT_DEFINED;
}

u32	File::close(){
	return NOT_DEFINED;
}

u32	File::read(u32 pos,u8* buffer,u32 size){
	return NOT_DEFINED;
}

u32	File::write(u32 pos,u8* buffer,u32 size){
	return NOT_DEFINED;
}

u32	File::ioctl(u32 id,u8* buffer){
	return NOT_DEFINED;
}

u32	File::remove(){
	delete this;
	return NOT_DEFINED;
}

stat_fs File::stat(){
	stat_fs st;
	return st;
}
//为映射的文件分配页表
u32 File::mmap(u32 sizee,u32 flags,u32 offset,u32 prot){
    if(map_memory!=NULL){
        int i=0;
        unsigned int adress;
        struct page *pg;
        process_st* current = (arch.pcurrent)->getPInfo();
        for(i=0;i<sizee;++i){
            adress = (unsigned int)(map_memory+i*PAGESIZE);
            //分配页表
            pg = (struct page *)kmalloc(sizeof(struct page));
            pg->p_addr = (char *)adress;
            pg->v_addr = (char*)(adress & 0xFFFFF000);
            //          new           old
            list_add(&pg->list,&current->pglist);//将页放入进程的页链表中
            pd_add_page(pg->v_addr,pg->p_addr, PG_USER,current->pd);//PG_USER表示用户级别
        }
        return (u32)map_memory;
    }else{
        return -1;
    }
}