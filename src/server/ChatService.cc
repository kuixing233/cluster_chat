#include "../../include/server/ChatService.h"
#include "../../include/public.h"

#include "muduo/base/Logging.h"

ChatService *ChatService::_chatServiceInstance = nullptr;

ChatService::ChatService()
{
    if (_redis.connect())
    {
        _redis.init_notify_handler(std::bind(ChatService::handlerRedisSubMsg, this, placeholders::_1, placeholders::_2));
    }

    // 注册登录事件的回调函数
    _msgHandlerMap.insert(make_pair<int, MsgHandler>(LOGIN_MSG, std::bind(&ChatService::login, this, placeholders::_1, placeholders::_2, placeholders::_3)));

    // 注册注册事件的回调函数
    _msgHandlerMap.insert(make_pair<int, MsgHandler>(REG_MSG, std::bind(&ChatService::reg, this, placeholders::_1, placeholders::_2, placeholders::_3)));

    // 注册一对一聊天事件的回调函数
    _msgHandlerMap.insert(make_pair<int, MsgHandler>(ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, placeholders::_1, placeholders::_2, placeholders::_3)));

    // 注册添加好友事件的回调函数
    _msgHandlerMap.insert(make_pair<int, MsgHandler>(ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, placeholders::_1, placeholders::_2, placeholders::_3)));

    // 注册建群事件的回调函数
    _msgHandlerMap.insert(make_pair<int, MsgHandler>(CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, placeholders::_1, placeholders::_2, placeholders::_3)));

    // 注册加群事件的回调函数
    _msgHandlerMap.insert(make_pair<int, MsgHandler>(ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, placeholders::_1, placeholders::_2, placeholders::_3)));

    // 注册群聊事件的回调函数
    _msgHandlerMap.insert(make_pair<int, MsgHandler>(GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, placeholders::_1, placeholders::_2, placeholders::_3)));

    // 注册退出登录事件的回调函数
    _msgHandlerMap.insert(make_pair<int, MsgHandler>(LOGINOUT_MSG, std::bind(&ChatService::logout, this, placeholders::_1, placeholders::_2, placeholders::_3)));
}

ChatService::~ChatService()
{
}

// 获得聊天服务器业务类的单例对象
ChatService *ChatService::getInstance()
{
    if (_chatServiceInstance == nullptr)
    {
        _chatServiceInstance = new ChatService();
    }
    return _chatServiceInstance;
}

// 销毁聊天服务器业务类的单例对象
void ChatService::destroyInstance()
{
    if (_chatServiceInstance)
    {
        delete _chatServiceInstance;
        _chatServiceInstance = nullptr;
    }
}

// 处理登录业务 id pwd == pwd
void ChatService::login(const TcpConnectionPtr &conn, Json &js, Timestamp time)
{
    int id = js["id"].get<int>();
    string pwd = js["password"];

    User user = _userModel.query(id);

    Json resJs;
    resJs["msgid"] = LOGIN_MSG_ACK;
    if (user.getId() == id && user.getPassword() == pwd)
    {
        // 该用户已经登陆，不允许重复登陆
        if (user.getState() == "online")
        {
            resJs["errno"] = 1;
            resJs["errmsg"] = "该用户已经登录，不允许重复登录";
        }
        else
        {
            // 登录成功
            // 记录用户连接信息，加锁保证线程安全
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({user.getId(), conn});
            }

            // redis订阅用户id相应通道
            _redis.subscribe(user.getId());

            // 发送好友列表
            vector<User> userVec = _friendModel.query(user.getId());
            if (!userVec.empty())
            {
                vector<string> userStrVec;
                for (auto user : userVec)
                {
                    Json tmpJs;
                    tmpJs["id"] = user.getId();
                    tmpJs["name"] = user.getName();
                    tmpJs["state"] = user.getState();
                    userStrVec.push_back(tmpJs.dump());
                }
                resJs["friends"] = userStrVec;
            }
            // 发送群组列表
            vector<Group> groupVec = _groupModel.queryGroups(user.getId());
            if (!groupVec.empty())
            {
                vector<string> groupStrVec;
                for (auto group : groupVec)
                {
                    Json tmpJs;
                    tmpJs["id"] = group.getId();
                    tmpJs["groupname"] = group.getGroupname();
                    tmpJs["groupdesc"] = group.getGroupdesc();

                    if (!group.getUser().empty())
                    {
                        vector<string> groupuserStrVec;
                        for (auto user : group.getUser())
                        {
                            Json tmpUserJs;
                            tmpUserJs["id"] = user.getId();
                            tmpUserJs["name"] = user.getName();
                            tmpUserJs["state"] = user.getState();
                            tmpUserJs["role"] = user.getRole();
                            groupuserStrVec.push_back(tmpUserJs.dump());
                        }
                        tmpJs["users"] = groupuserStrVec;
                    }
                    groupStrVec.push_back(tmpJs.dump());
                }
                resJs["groups"] = groupStrVec;
            }
            // 显示登录用户的离线消息
            vector<string> offlineVec = _offlineModel.query(user.getId());
            if (!offlineVec.empty())
            {
                resJs["offlinemsg"] = offlineVec;
                _offlineModel.remove(user.getId());
            }
            // 更新用户状态信息 state offline -> online
            user.setState("online");
            _userModel.updateState(user);
            resJs["errno"] = 0;
            resJs["name"] = user.getName();
            resJs["id"] = user.getId();
        }
    }
    // 用户名不存在或者密码错误
    else
    {
        resJs["errno"] = 2;
        resJs["errmsg"] = "用户名或密码错误";
    }
    conn->send(resJs.dump());
}

// 处理注册业务
void ChatService::reg(const TcpConnectionPtr &conn, Json &js, Timestamp time)
{
    User user;
    string name = js["name"];
    string password = js["password"];

    user.setName(js["name"]);
    user.setPassword(js["password"]);

    bool bl = _userModel.insert(user);
    Json resJs;
    if (bl)
    {
        resJs["msgid"] = REG_MSG_ACK;
        resJs["errno"] = 0;
        resJs["id"] = user.getId();
    }
    else
    {
        resJs["msgid"] = REG_MSG_ACK;
        resJs["errno"] = 1;
    }
    conn->send(resJs.dump());
}

// 处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    // 注意线程安全
    // 从map表中删除用户的连接信息
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                user.setId(it->first);
                _userConnMap.erase(it->first);
            }
        }
    }

    // 在redis中取消订阅通道
    _redis.unsubscribe(user.getId());

    // 更新用户的状态信息
    if (user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
}

// 重置服务器状态
void ChatService::reset()
{
    _userModel.reset();
}

// 处理一对一聊天业务
void ChatService::oneChat(const TcpConnectionPtr &conn, Json &js, Timestamp time)
{
    int fromid = js["id"];
    int toid = js["toid"];
    string msg = js["msg"];

    auto it = _userConnMap.find(toid);
    if (it != _userConnMap.end())
    {
        // toId 在线，转发消息，服务器主动推送消息给toid用户
        it->second->send(js.dump());
        return;
    }

    // 从数据库中查询toid是不是真的不在线
    // 如果用户在其他服务器上在线，发布订阅
    User user = _userModel.query(toid);
    if (user.getState() == "online")
    {
        _redis.publish(toid, js.dump());
        return;
    }

    // toId 不在线，存储离线消息
    _offlineModel.insert(toid, js.dump());
}

// 处理添加好友业务
void ChatService::addFriend(const TcpConnectionPtr &conn, Json &js, Timestamp time)
{
    int userid = js["id"];
    int friendid = js["friendid"];

    if (!_friendModel.insert(userid, friendid))
    {
        LOG_ERROR << "add user error";
    }
}

// 处理创建群业务
void ChatService::createGroup(const TcpConnectionPtr &conn, Json &js, Timestamp time)
{
    int id = js["id"].get<int>();
    string groupName = js["groupname"];
    string groupDesc = js["groupdesc"];

    Group group;
    group.setGroupname(groupName);
    group.setGroupdesc(groupDesc);

    _groupModel.createGroup(group);
    _groupModel.addGroup(id, group.getId(), "creator");
}

// 处理添加群业务
void ChatService::addGroup(const TcpConnectionPtr &conn, Json &js, Timestamp time)
{
    int id = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();

    _groupModel.addGroup(id, groupid, "normal");
}

// 处理发送群消息业务
void ChatService::groupChat(const TcpConnectionPtr &conn, Json &js, Timestamp time)
{
    int id = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    string msg = js["msg"];

    vector<int> userIdVec = _groupModel.queryGroupUsers(id, groupid);
    for (int uid : userIdVec)
    {
        auto it = _userConnMap.find(uid);
        if (it != _userConnMap.end())
        {
            it->second->send(js.dump());
        }
        else
        {
            // 从数据库中查询toid是不是真的不在线
            // 如果用户在其他服务器上在线，发布订阅
            User user = _userModel.query(uid);
            if (user.getState() == "online")
            {
                _redis.publish(uid, js.dump());
                return;
            }
            _offlineModel.insert(uid, js.dump());
        }
    }
}

// 处理用户退出业务
void ChatService::logout(const TcpConnectionPtr &conn, Json &js, Timestamp time)
{
    int id = js["id"];

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(id);
        }
    }

    // 在redis中取消订阅该用户的通道
    _redis.unsubscribe(id);

    User user;
    user.setId(id);
    user.setState("offline");
    _userModel.updateState(user);
}

// 从redis消息队列中获取订阅的消息
void ChatService::handlerRedisSubMsg(int id, string msg)
{
    lock_guard<mutex> lock(_connMutex);

    // 存储该用户的离线消息，防止在从redis读取订阅的时间内用户离线
    auto it = _userConnMap.find(id);
    if (it != _userConnMap.end())
    {
        it->second->send(msg);
        return;
    }

    _offlineModel.insert(id, msg);
}

MsgHandler ChatService::getHandler(int msgid)
{
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        // 该消息类型没有对应的回调函数
        // 返回一个空的回调函数并记录日志
        return [=](const TcpConnectionPtr &conn, Json &js, Timestamp time)
        {
            LOG_ERROR << "msgid " << msgid << " can not find handler!";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}