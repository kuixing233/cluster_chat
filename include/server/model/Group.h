#ifndef __GROUP_H__
#define __GROUP_H__

#include "GroupUser.h"

#include <string>
#include <vector>
using namespace std;

class Group
{
public:
    Group(int id = -1, string groupname = "", string groupdesc = "")
    : _id(id)
    , _groupname(groupname)
    , _groupdesc(groupdesc)
    {

    }

    int getId() {return _id;}
    string getGroupname() {return _groupname;}
    string getGroupdesc() {return _groupdesc;}
    vector<GroupUser> & getUser() {return _groupusers;}

    void setId(int id) {_id = id;}
    void setGroupname(string name) {_groupname = name;}
    void setGroupdesc(string desc) {_groupdesc = desc;}

private:
    int _id;
    string _groupname;
    string _groupdesc;
    vector<GroupUser> _groupusers;
};

#endif