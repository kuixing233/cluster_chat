#ifndef __FRIENDMODEL_H__
#define __FRIENDMODEL_H__

#include "db/db.h"
#include "User.h"

#include <vector>

using namespace std;

class FriendModel
{
public:
    FriendModel();
    ~FriendModel();

    // 向friend表添加一条记录
    bool insert(int userid, int friendid);

    // 返回该用户的好友名称
    vector<User> query(int userid);

private:
    MySql _mysqlObj;
};

#endif
