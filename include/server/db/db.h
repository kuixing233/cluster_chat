#ifndef __DB_H__
#define __DB_H__

#include <mysql/mysql.h>

#include <string>

using namespace std;

class MySql
{
public:
    // 初始化数据库连接
    MySql();

    // 释放数据库连接资源
    ~MySql();

    // 连接数据库
    bool connect();

    // 更新操作
    bool update(string sql);

    // 查询操作
    MYSQL_RES * query(string sql);

    // 得到mysql连接指针
    MYSQL * getConnection();

private:
    MYSQL * _conn;
};

#endif