#include "../../../include/server/model/FriendModel.h"
#include "../../../include/server/model/User.h"

#include "muduo/base/Logging.h"
using namespace muduo;
    
FriendModel::FriendModel()
{
    _mysqlObj.connect();
}

FriendModel::~FriendModel()
{

}

// 向friend表添加一条记录
bool FriendModel::insert(int userid, int friendid)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into friend (userid, friendid) values (%d, %d)", userid, friendid);

    if (_mysqlObj.update(sql))
    {
        LOG_INFO << "add friend success => sql: " << sql;
        return true;
    }
    else
    {
        LOG_ERROR<< "add friend error => sql: " << sql;
    }
    return false;
}

// 返回该用户的好友名称
vector<User> FriendModel::query(int userid)
{
    char sql[1024] = {0};
    sprintf(sql, "select a.id, a.name, a.state from user a inner join friend b on b.friendid = a.id where b.userid = %d", userid);

    MYSQL_RES * res = _mysqlObj.query(sql);
    vector<User> vec;
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            User user;
            user.setId(atoi(row[0]));
            user.setName(row[1]);
            user.setState(row[2]);
            vec.push_back(user);
        }
        mysql_free_result(res);
    }
    return vec;
}