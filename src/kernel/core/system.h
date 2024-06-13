#ifndef SYSTEM_H
#define SYSTEM_H

#include <runtime/types.h>
#include <core/env.h>
#include <core/user.h>

class System
{
public:
    System();
    ~System();

    void init();
    char *getvar(char *name);//获取环境变量，返回变量值。

    void addUserToList(User *us);//将用户添加到用户列表。

    User *getUser(char *nae);//根据用户名获取用户对象

    int login(User *us, char *pass);//用户登录，验证用户名和密码。
    u32 isRoot(); // 检查当前用户是否为 root 用户，返回 1 表示是 root 用户。

private:
    User *listuser;

    File *var;//变量文件

    User *actual;
    User *root;

    Variable *uservar;
};

extern System sys;
#endif