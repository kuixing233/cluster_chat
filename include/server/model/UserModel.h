#ifndef __USERMODEL_H__
#define __USERMODEL_H__

#include "muduo/base/Logging.h"
using namespace muduo;

#include "./db/db.h"
#include "User.h"

class UserModel
{
public:
    UserModel();
    ~UserModel();
    // 插入一个新用户
    bool insert(User & user);

    // 通过id查找User信息
    User query(int id);

    // 更新用户状态信息
    bool updateState(User user);

    // 将所有在线用户状态修改为离线
    void reset();

private:
    MySql _mysqlObj;
};


#endif