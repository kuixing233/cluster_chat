#ifndef __CHATSERVER_H__
#define __CHATSERVER_H__

#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"

using namespace muduo;
using namespace muduo::net;

class ChatServer
{
public:
    // 构造TcpServer对象
    ChatServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg);
    
    ~ChatServer();

    // 启动服务器
    void start();

private:

    // 注册连接到来时的回调函数
    void onConnection(const TcpConnectionPtr&);

    // 注册发生读写事件时的回调函数
    void onMessage(const TcpConnectionPtr&,
                            Buffer*,
                            Timestamp);
private:
    TcpServer _server;
    EventLoop * _loop;
};

#endif