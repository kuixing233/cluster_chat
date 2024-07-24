#include "../../../include/server/model/UserModel.h"

UserModel::UserModel()
{
    _mysqlObj.connect();
}

UserModel::~UserModel()
{

}

bool UserModel::insert(User & user)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into user (name, password, state) values ('%s', '%s', '%s')", 
    user.getName().c_str(),
    user.getName().c_str(),
    user.getState().c_str());

    if (_mysqlObj.update(sql))
    {
        LOG_INFO << "add user success => sql: " << sql;
        user.setId(mysql_insert_id(_mysqlObj.getConnection()));
        return true;                 
    }
    LOG_INFO << "add user error => sql: " << sql;
    return false;
}

User UserModel::query(int id)
{
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = %d", id);

    MYSQL_RES * res = _mysqlObj.query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row != nullptr)
        {
            User user;
            user.setId(atoi(row[0]));
            user.setName(row[1]);
            user.setPassword(row[2]);
            user.setState(row[3]);
            mysql_free_result(res);
            return user;
        }
    }
    return User();
}

bool UserModel::updateState(User user)
{
    char sql[1024] = {0};
    sprintf(sql, "update user set state = '%s' where id = %d", user.getState().c_str(), user.getId());   

    if (_mysqlObj.update(sql))
    {
        LOG_INFO << "user: " << user.getId() << " state change success -> " << user.getState();
        return true;
    }    
    LOG_INFO << "user: " << user.getId() << " state change error";
    return false;
}

// 将所有在线用户状态修改为离线
void UserModel::reset()
{
    char sql[1024] = {0};
    sprintf(sql, "update user set state = 'offline' where state = 'online'");   

    _mysqlObj.update(sql);
}