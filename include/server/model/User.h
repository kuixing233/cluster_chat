#ifndef __USER_H__
#define __USER_H__

#include <string>

using namespace std;

class User 
{
public:
    User(int id = -1, string name = "", string password = "", string state = "offline")
    : _id(id)
    , _name(name)
    , _password(password)
    , _state(state)
    {

    }

    ~User() {};

    void setId(int id) {_id = id;}
    void setName(string name) {_name = name;}
    void setPassword(string password) {_password = password;}
    void setState(string state) {_state = state;}

    int getId() {return _id;}
    string getName() {return _name;}
    string getPassword() {return _password;}
    string getState() {return _state;}

protected:
    int _id;
    string _name;
    string _password;
    string _state;
};

#endif