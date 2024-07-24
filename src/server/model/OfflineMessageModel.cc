#include "../../../include/server/model/OfflineMessageModel.h"

#include "muduo/base/Logging.h"
using namespace muduo;

OfflineMessageModel::OfflineMessageModel()
{
    _mysqlObj.connect();
}

OfflineMessageModel::~OfflineMessageModel()
{
}

// 插入一条离线消息
bool OfflineMessageModel::insert(int id, string msg)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage (userid, message) values (%d, '%s')", id, msg.c_str());

    if (_mysqlObj.update(sql))
    {
        LOG_INFO << "add offline message success => sql: " << sql;
        return true;
    }
    else
    {
        LOG_ERROR << "add offline message error => sql: " << sql;
    }
    return false;
}

// 查询该用户是否有离线消息
vector<string> OfflineMessageModel::query(int id)
{
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid = %d", id);

    MYSQL_RES *res = _mysqlObj.query(sql);
    vector<string> vec;
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            vec.push_back(row[0]);
        }
        mysql_free_result(res);
    }
    return vec;
}

// 删除该用户所有的离线消息
bool OfflineMessageModel::remove(int id)
{
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid = %d", id);

    if (_mysqlObj.update(sql))
    {
        LOG_INFO << "delete offline message success => sql: " << sql;
        return true;
    }
    else
    {
        LOG_ERROR << "delete offline message error => sql: " << sql;
    }
    return false;
}