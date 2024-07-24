#include "../../../include/server/db/db.h"

#include "muduo/base/Logging.h"

static string server = "127.0.0.1";
static string user = "root";
static string password = "123";
static string dbname = "chat";

// 初始化数据库连接
MySql::MySql()
{
    _conn = mysql_init(nullptr);
}

// 释放数据库连接资源
MySql::~MySql()
{
    if (_conn != nullptr)
    {
        mysql_close(_conn); 
    } 
}

// 连接数据库
bool MySql::connect()
{
    MYSQL * p = mysql_real_connect(_conn, server.c_str(), user.c_str(), password.c_str(), dbname.c_str(),
                        3306, nullptr, 0);
    if (p != nullptr)
    {
        mysql_query(_conn, "set names utf-8");
        LOG_INFO << "connect MySql success!";
    }
    else
    {
        LOG_INFO << "connect MySql error!";
    }
    return p;
}

// 更新操作
bool MySql::update(string sql)
{
    if (mysql_query(_conn, sql.c_str()) != 0)
    {
        LOG_ERROR << __FILE__ << ":" << __LINE__ << ":"
                    << sql << " 更新失败！" << "error msg: " << mysql_error(_conn);
        return false;
    }
    return true;
}

// 查询操作
MYSQL_RES * MySql::query(string sql)
{
    if (mysql_query(_conn, sql.c_str()) != 0)
    {
        LOG_ERROR << __FILE__ << ":" << __LINE__ << ":"
                << sql << " 查询失败！";
        return nullptr;
    }
    return mysql_use_result(_conn);
}

// 得到mysql连接指针
MYSQL * MySql::getConnection()
{
    return _conn;
}