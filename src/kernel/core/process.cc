#include <core/os.h>
#include <api/dev/proc.h>


char* Process::default_tty="/dev/tty";

u32 Process::proc_pid= 0;

Process::~Process(){
    delete ipc;
    arch.change_process_father(this,pparent);//从进程链表中删除这个进程
}

Process::Process(char *n) : File(n,TYPE_PROCESS)
{
    fsm.addFile("/proc/",this);
    pparent=arch.pcurrent;
    pid=proc_pid;
    ++proc_pid;//进程id一次增加

    info.pid=pid;

    if(pparent!=NULL)
    {
        cdir=pparent->getCurrentDir();
    }else
    {
        cdir=fsm.getRoot();
    }
    arch.addProcess(this);
    info.vinfo=(void*)this;
    int i;

    for(i=0;i<CONFIG_MAX_FILE;++i)
    {
        openfp[i].fp =NULL;
    }
    ipc = new Buffer();
}




u32 Process::open(u32 flag)
{
    return RETURN_OK;
}


u32	Process::close(){
	return RETURN_OK;
}

Process* Process::getPParent(){
	return pparent;
}

void Process::setPParent(Process*p){
	pparent=p;
}

u32 Process::read(u32 pos,u8* buffer,u32 sizee){
    u32 ret=RETURN_OK;
    arch.enable_interrupt();
    while(ipc->isEmpty());
    ret=ipc->get(buffer,sizee);

    arch.disable_interrupt();
    return ret;
}


u32 Process::write(u32 pos,u8 *buffer,u32 sizee)
{
    ipc->add(buffer,sizee);
    return size;
}


u32 Process::ioctl(u32 id, u8 *buffer)
{
	u32 ret;
	switch (id)
	{
	case API_PROC_GET_PID:
		ret = pid;
		break;

	case API_PROC_GET_INFO:
		reset_pinfo();
		memcpy((char *)buffer, (char *)&ppinfo, sizeof(proc_info));
		break;

	default:
		ret = NOT_DEFINED;
		break;
	}

	return ret;
}

int Process::fork()
{
    Process* p = new Process("fork_child");

    // 调用 arch.fork 返回值表示是否是子进程
    int ret = arch.fork(p->getPInfo(), &info);

    // 在子进程中，arch.fork 返回 0
    if (ret == 0) {
        // 子进程的处理逻辑，可以在这里添加
        return 0;
    } else if (ret > 0) {
        // 父进程的处理逻辑
        if (pparent != NULL) {
            pparent->sendSignal(SIGCHLD);
        }
        // 返回子进程的 PID
        return ret;
    } else {
        // 如果 fork 失败，清理新进程对象并返回错误码
        delete p;
        return -1;
    }
}



u32 Process::wait()
{
	arch.enable_interrupt();
	while (is_signal(info.signal, SIGCHLD) == 0)
		;
	clear_signal(&(info.signal), SIGCHLD);
	arch.destroy_all_zombie();
	arch.disable_interrupt();
	return 1;
}



u32 Process::remove()
{
	delete this;
	return RETURN_OK;
}

void Process::scan()
{
}


void Process::sendSignal(int sig)
{
	set_signal(&(info.signal), sig); // 子进程发送给父进程的信号
}



void Process::reset_pinfo()
{
	strncpy(ppinfo.name, name, 32);
	ppinfo.pid = pid;
	ppinfo.tid = 0;
	ppinfo.state = state;
	ppinfo.vmem = 10 * 1024 * 1024;
	ppinfo.pmem = 10 * 1024 * 1024;
}

