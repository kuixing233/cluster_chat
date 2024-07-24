#ifndef __OFFLINEMESSAGEMODEL_H__
#define __OFFLINEMESSAGEMODEL_H__

#include "db/db.h"

#include <vector>
using namespace std;

class OfflineMessageModel
{
public:
    OfflineMessageModel();
    ~OfflineMessageModel();

    // 插入一条离线消息
    bool insert(int id, string msg);

    // 查询该用户是否有离线消息
    vector<string> query(int id); 

    // 删除该用户所有的离线消息
    bool remove(int id);

private:
    MySql _mysqlObj;
};

#endif