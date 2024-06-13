#ifndef USER_H
#define USER_H

#include <core/file.h>
#include <runtime/list.h>

enum
{
    USER_ROOT, // root
    USER_NORM  // utilisateur normal普通用户
};

class User : public File
{
public:
    User(char *n);
    ~User();

    u32 open(u32 flag);
    u32 close();
    u32 read(u8 *buffer, u32 size);
    u32 write(u8 *buffer, u32 size);
    u32 ioctl(u32 id, u8 *buffer);
    u32 remove();
    void scan();

    void setPassword(char *n);
    char *getPassword();

    User *getUNext();//获取下一个 User 对象的指针
    void setUNext(User *us);//设置下一个 User 对象的指针

    void	setUType(u32 t);//设置用户的类型
	u32		getUType();//获取用户的类型，返回用户类型值
		
protected:
    u32 utype;

    User* unext;
    char password[512];
};

#endif