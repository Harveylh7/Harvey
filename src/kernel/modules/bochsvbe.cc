//实现了一个驱动程序类 Bochs，用于管理 VBE 设备的操作


#include <core/os.h>
#include <bochsvbe.h>

/* Video driver for VBE/Bios */

static void BgaWriteRegister(unsigned short IndexValue, unsigned short DataValue)
{
    io.outw(VBE_DISPI_IOPORT_INDEX, IndexValue); // 写入索引值到索引端口
    io.outw(VBE_DISPI_IOPORT_DATA, DataValue);   // 写入数据值到数据端口
}

static unsigned short BgaReadRegister(unsigned short IndexValue)
{
    io.outw(VBE_DISPI_IOPORT_INDEX, IndexValue); // 写入索引值到索引端口
    return io.inw(VBE_DISPI_IOPORT_DATA);        // 从数据端口读取数据值
}

static int BgaIsAvailable(void)
{
    return (BgaReadRegister(VBE_DISPI_INDEX_ID) == VBE_DISPI_ID4); // 检查Bochs VGA设备是否可用
}

static void BgaSetVideoMode(unsigned int Width, unsigned int Height, unsigned int BitDepth, int UseLinearFrameBuffer, int ClearVideoMemory)
{
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED); // 禁用当前模式
    BgaWriteRegister(VBE_DISPI_INDEX_XRES, Width); // 设置水平分辨率
    BgaWriteRegister(VBE_DISPI_INDEX_YRES, Height); // 设置垂直分辨率
    BgaWriteRegister(VBE_DISPI_INDEX_BPP, BitDepth); // 设置颜色深度
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | 
        (UseLinearFrameBuffer ? VBE_DISPI_LFB_ENABLED : 0) | 
        (ClearVideoMemory ? 0 : VBE_DISPI_NOCLEARMEM)); // 启用新的模式
}

static void BgaSetBank(unsigned short BankNumber)
{
    BgaWriteRegister(VBE_DISPI_INDEX_BANK, BankNumber); // 设置显存银行
}

File* bochs_mknod(char* name, u32 flag, File* dev)
{
    Bochs* cons = new Bochs(name); // 创建新的Bochs设备实例
    return cons; // 返回新创建的设备实例
}

module("module.bvbe", MODULE_DEVICE, Bochs, bochs_mknod) // 初始化一个Bochs模块

Bochs::~Bochs()
{
    // 析构函数，用于资源清理（此处为空）
}

Bochs::Bochs(char* n) : Device(n)
{
    fbinfo_best.w = 1024; // 设置最佳帧缓冲宽度
    fbinfo_best.h = 768; // 设置最佳帧缓冲高度
    fbinfo_best.bpp = 32; // 设置最佳帧缓冲的颜色深度
    fbinfo_best.state = FB_ACTIVE; // 设置帧缓冲状态为活跃
    fbinfo_best.vmem = (unsigned int*)VBE_DISPI_LFB_PHYSICAL_ADDRESS; // 设置帧缓冲的物理地址
    
    fbinfo.w = 0;
    fbinfo.h = 0;
    fbinfo.bpp = 0;
    fbinfo.state = FB_NOT_ACTIVE;
    fbinfo.vmem = (unsigned int*)VBE_DISPI_LFB_PHYSICAL_ADDRESS; // 设置当前帧缓冲的物理地址
    map_memory = (char*)VBE_DISPI_LFB_PHYSICAL_ADDRESS; // 映射内存地址
}

u32 Bochs::open(u32 flag)
{
    return RETURN_OK; // 打开设备，返回成功
}

u32 Bochs::read(u32 pos, u8* buffer, u32 size)
{
    return NOT_DEFINED; // 读操作未定义
}

u32 Bochs::write(u32 pos, u8* buffer, u32 size)
{
    return NOT_DEFINED; // 写操作未定义
}

u32 Bochs::close()
{
    return RETURN_OK; // 关闭设备，返回成功
}

void Bochs::scan()
{
    // 扫描设备（此处为空）
}

u32 Bochs::ioctl(u32 id, u8* buffer)
{
    u32 ret = 0;
    switch (id)
    {
        case DEV_GET_TYPE:
            ret = DEV_TYPE_FB; // 返回设备类型为帧缓冲
            break;

        case DEV_GET_STATE:
            ret = DEV_STATE_OK; // 返回设备状态为正常
            break;

        case DEV_GET_FORMAT:
            ret = DEV_FORMAT_FB; // 返回设备格式为帧缓冲
            break;

        case API_FB_IS_AVAILABLE:
            ret = (u32)BgaIsAvailable(); // 检查Bochs VGA设备是否可用
            break;

        case API_FB_GET_INFO:
            memcpy((char*)buffer, (char*)&fbinfo, sizeof(fb_info)); // 获取当前帧缓冲信息
            break;

        case API_FB_SET_INFO:
            memcpy((char*)&fbinfo, (char*)buffer, sizeof(fb_info)); // 设置当前帧缓冲信息
            BgaSetVideoMode(fbinfo.w, fbinfo.h, (unsigned int)fbinfo.bpp / 8, 1, 1); // 设置视频模式
            break;

        case API_FB_GET_BINFO:
            memcpy((char*)buffer, (char*)&fbinfo_best, sizeof(fb_info)); // 获取最佳帧缓冲信息
            break;

        default:
            ret = NOT_DEFINED; // 未定义的ioctl操作
            break;
    }
    return ret; // 返回操作结果
}

u32 Bochs::remove()
{
    delete this; // 删除当前对象
    return RETURN_OK; // 返回成功
}