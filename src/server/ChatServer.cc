#include "../../include/server/ChatServer.h"
#include "../../include/server/ChatService.h"

#include "../../thirdparty/json.hpp"
using Json = nlohmann::json;

#include <iostream>
#include <string>
#include <functional>

using namespace std;

// 构造TcpServer对象
ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 给服务器注册用户连接的创建和断开回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, placeholders::_1));

    // 给服务器注册用户读写事件回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));

    // 设置服务端的线程数量：1个I/O线程，3个worker线程
    _server.setThreadNum(4);
}

ChatServer::~ChatServer()
{
}

// 启动服务器
void ChatServer::start()
{
    _server.start();
}

// 连接到来时的回调函数
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    // 客户端断开连接
    if (!conn->connected())
    {
        ChatService::getInstance()->clientCloseException(conn);
        conn->shutdown();
    }
}

// 发生读写事件时的回调函数
void ChatServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buffer,
                           Timestamp time)
{
    string buf = buffer->retrieveAllAsString();

    // 数据的反序列化
    Json js = Json::parse(buf);
    
    // 目的：将网络模块的代码和业务模块的代码解耦

    // 通过msgid获取对应的业务handler
    auto msgHandler = ChatService::getInstance()->getHandler(js["msgid"].get<int>()); 
    // 回调消息绑定相应的事件处理器，来执行相应的业务处理
    msgHandler(conn, js, time);
}

