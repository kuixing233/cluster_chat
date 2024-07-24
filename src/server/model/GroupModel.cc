#include "../../../include/server/model/GroupModel.h"

#include "muduo/base/Logging.h"
using namespace muduo;


GroupModel::GroupModel()
{
    _mysqlObj.connect();
}

GroupModel::~GroupModel()
{

}

// 创建群组
bool GroupModel::createGroup(Group &group)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into allgroup(groupname, groupdesc) values ('%s', '%s')", group.getGroupname().c_str(), group.getGroupdesc().c_str());

    if (_mysqlObj.update(sql))
    {
        group.setId(mysql_insert_id(_mysqlObj.getConnection()));
        LOG_INFO << "add offline message success => sql: " << sql;
        return true;
    }
    LOG_ERROR<< "add offline message error => sql: " << sql;
    return false;
}

// 加入群组
void GroupModel::addGroup(int userid, int groupid, string role)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser(groupid, userid, grouprole) values (%d, %d, '%s')", groupid, userid, role.c_str());

    if (_mysqlObj.update(sql))
    {
        LOG_INFO << "add offline message success => sql: " << sql;
    }
    else
    {
        LOG_ERROR<< "add offline message error => sql: " << sql;
    }
}

// 查询用户所在群组信息
vector<Group> GroupModel::queryGroups(int userid)
{
    char sql[1024] = {0};
    sprintf(sql, "select a.id, a.groupname, a.groupdesc from allgroup a inner join groupuser b on b.groupid = a.id where b.userid = %d", userid);

    MYSQL_RES * res = _mysqlObj.query(sql);
    vector<Group> groupVec;
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            Group group;
            group.setId(atoi(row[0]));
            group.setGroupname(row[1]);
            group.setGroupdesc(row[2]);
            groupVec.push_back(group);
        }
        mysql_free_result(res);
    }

    for (Group & group : groupVec)
    {
        char sql[1024] = {0};
        sprintf(sql, "select a.id, a.name, a.state, b.grouprole from user a inner join groupuser b on b.userid = a.id where b.groupid = %d", group.getId());

        MYSQL_RES * res = _mysqlObj.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                GroupUser groupUser;
                groupUser.setId(atoi(row[0]));
                groupUser.setName(row[1]);
                groupUser.setState(row[2]);
                groupUser.setRole(row[3]);
                group.getUser().push_back(groupUser);
            }
            mysql_free_result(res);
        }
    }
    return groupVec;
}

// 根据指定的groupid查询群组用户id列表，除userid自己，主要用于群聊业务给群组其他成员群发消息
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where groupid = %d and userid != %d;", groupid, userid);

    MYSQL_RES * res = _mysqlObj.query(sql);
    vector<int> vec;
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            vec.push_back(atoi(row[0]));
        }
        mysql_free_result(res);
    }
    return vec;
}
