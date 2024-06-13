#ifndef __API_IOCTL__
#define __API_IOCTL__

// IOCTL控制命令
#define DEV_GET_TYPE        0x01    /* 获取设备类型 */
#define DEV_GET_STATE       0x02    /* 获取设备状态 */
#define DEV_GET_FORMAT      0x03    /* 获取设备格式 */

// 设备类型
#define DEV_TYPE_TTY        0x01    /* 终端设备 */
#define DEV_TYPE_DISK       0x02    /* 磁盘设备 */
#define DEV_TYPE_FB         0x03    /* 帧缓冲设备 */
#define DEV_TYPE_HID        0x04    /* 人机接口设备 */

// 设备格式
#define DEV_FORMAT_CHAR     0x01    /* 字符型设备 */
#define DEV_FORMAT_BLOCK    0x02    /* 块型设备 */
#define DEV_FORMAT_FB       0x03    /* 帧缓冲型设备 */

// 设备状态
#define DEV_STATE_OK        0x01    /* 设备正常 */
#define DEV_STATE_NOTREADY  0x02    /* 设备未就绪 */

#endif
