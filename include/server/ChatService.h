#ifndef __CHATSERVICE_H__
#define __CHATSERVICE_H__

#include "muduo/net/TcpConnection.h"
using namespace muduo;
using namespace muduo::net;

#include "../../thirdparty/json.hpp"
using Json = nlohmann::json;

#include "../model/UserModel.h"
#include "../model/OfflineMessageModel.h"
#include "../model/FriendModel.h"
#include "../model/GroupModel.h"
#include "../redis/redis.h"

#include <functional>
#include <unordered_map>
#include <mutex>
using namespace std;

// 不同消息类型的回调函数
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, Json &js, Timestamp time)>;

// 聊天服务器业务类
class ChatService
{
public:
    // 获得聊天服务器业务类的单例对象
    static ChatService * getInstance();
    // 销毁聊天服务器业务类的单例对象
    static void destroyInstance();

    // 处理登录业务
    void login(const TcpConnectionPtr &conn, Json &js, Timestamp time);

    // 处理注册业务
    void reg(const TcpConnectionPtr &conn, Json &js, Timestamp time);

    // 处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);

    // 重置服务器状态
    void reset();

    // 处理一对一聊天业务
    void oneChat(const TcpConnectionPtr &conn, Json &js, Timestamp time);

    // 处理添加好友业务
    void addFriend(const TcpConnectionPtr &conn, Json &js, Timestamp time);

    // 处理创建群业务
    void createGroup(const TcpConnectionPtr &conn, Json &js, Timestamp time);

    // 处理添加群业务
    void addGroup(const TcpConnectionPtr &conn, Json &js, Timestamp time);

    // 处理发送群消息业务
    void groupChat(const TcpConnectionPtr &conn, Json &js, Timestamp time);

    // 处理用户退出业务
    void logout(const TcpConnectionPtr &conn, Json &js, Timestamp time);

    // 从redis消息队列中获取订阅的消息
    void handlerRedisSubMsg(int id, string msg);
    /*
        注意：
            发送消息时如果发现用户在线（发送方和接收方登录在同一台服务器上）直接发送消息
                     如果用户不在线，则去数据库查询接收方是否在其他服务器上登录，如果在线，则在redis相应通道上发布订阅
                                    否则存储离线消息
            从消息队列接收消息则直接使用回调函数，因为在成功登陆后订阅了该用户id的通道
    */

    MsgHandler getHandler(int msgid);
    
private:
    ChatService();
    ~ChatService();

private:
    // 存储消息id和其对应的业务处理方法
    unordered_map<int, MsgHandler> _msgHandlerMap; 

    // 存储在线用户的通信连接
    unordered_map<int, TcpConnectionPtr> _userConnMap;

    // 用户通信连接的互斥锁对象
    mutex _connMutex;

    // 单例类对象
    static ChatService * _chatServiceInstance;

    // 用户表Model类对象
    UserModel _userModel;

    // 离线消息表Model类对象
    OfflineMessageModel _offlineModel;

    // 好友表Model类对象
    FriendModel _friendModel;    

    // 群组表Model类对象
    GroupModel _groupModel;    

    // redis操作类对象
    Redis _redis;
};


#endif
