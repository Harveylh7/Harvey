#ifndef __API_KEYBOARD__
#define __API_KEYBOARD__





//keyboard
enum {
    KEY_TAB = 7,
    KEY_BACKSPACE = 8,
    KEY_ENTER = 10,
    KEY_ESCAPE = 27,
    KEY_F1 = 255,
    KEY_F2 = 254,
    KEY_F3 = 253,
    KEY_F4 = 252,
    KEY_F5 = 251,
    KEY_F6 = 250,
    KEY_F7 = 249,
    KEY_F8 = 248,
    KEY_F9 = 247,
    KEY_F10 = 246,
    KEY_F11 = 245,
    KEY_F12 = 244
};

#define TABLE_KEYBOARD_SIZE			388  //键盘表的大小，包括了一些功能按键

#define API_KEYBOARD_SET_TABLE		0x4122 // 定义了一个用于设置键盘表的API命令码为0x4122。用于向系统传递键盘按键映射关系的数据，以便系统能够正确识别用户按下的键盘按键。



#endif