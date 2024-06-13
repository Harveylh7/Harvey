#ifndef __API_CLOCK__
#define __API_CLOCK__

// 时钟数据类型
typedef unsigned int clock_d;

// 时钟信息结构体
struct clock_info {
    clock_d h;      // 小时
    clock_d m;      // 分钟
    clock_d s;      // 秒
    
    clock_d day;    // 天
    clock_d month;  // 月
    clock_d year;   // 年
};

// API调用：获取时钟信息
#define API_CLOCK_GET_INFO      0x6122

#endif
