#ifndef __GROUPUSER_H__
#define __GROUPUSER_H__

#include "User.h"

class GroupUser
: public User
{
public:
    string getRole() {return _role;}
    void setRole(string role) {_role = role;}

private:
    string _role;
};


#endif