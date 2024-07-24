#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"

using namespace muduo;
using namespace muduo::net;

#include <iostream>
#include <functional>
#include <string>
using namespace std;

/*
基于muduo库开发服务器程序：
    1. 创建TcpServer对象
    2. 创建EventLoop事件循环对象的指针
    3. 明确TcpServer构造函数需要什么参数，输出EchoServer的构造函数
    4. 在当前服务器类构造函数中，注册处理连接的回调函数和处理读写时间的构造函数
    5. 设置合适的服务端线程数量，muduo库会自己分配I/O线程和worker线程
*/

class EchoServer
{
public:
    EchoServer(EventLoop* loop,  // 事件循环
            const InetAddress& listenAddr,  // IP + port
            const string& nameArg)  // 服务器的名字
    : _server(loop, listenAddr, nameArg)
    , _loop(loop)
    {
        // 给服务器注册用户连接的创建和断开回调
        _server.setConnectionCallback(bind(&EchoServer::onConnection, this, placeholders::_1));

        // 给服务器注册用户读写事件回调
        _server.setMessageCallback(bind(&EchoServer::onMessage, this, placeholders::_1, placeholders::_2, placeholders::_3));

        // 设置服务端的线程数量：1个I/O线程，3个worker线程
        _server.setThreadNum(4);
    }

    // 开启事件循环
    void start()
    {
        _server.start();
    }

private:
    // 专门处理用户的连接创建和断开
    void onConnection(const TcpConnectionPtr & conn)
    {
        if (conn->connected())
        {
            cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort()
                    << " stat: online" << endl;
        }
        else
        {
            cout << "stat: offline" << endl;
        }
    }

    // 专门处理用户的读写事件
    void onMessage(const TcpConnectionPtr & conn,   // 连接
                            Buffer* buffer,    // 缓冲区
                            Timestamp time)     // 接收到数据的时间信息
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data: " << buf << " time: " << time.toFormattedString() << endl; 
    }

private:
    TcpServer _server;
    EventLoop * _loop;
};

int main()
{
    EventLoop loop; // epoll
    InetAddress addr("127.0.0.1", 6000);

    EchoServer echoServer(&loop, addr, "EchoServer");
    echoServer.start(); // listenfd epoll_ctl=>epoll
    loop.loop(); // epoll_wait以阻塞的方式等待新用户的连接，已连接用户的读写事件等

    return 0;
}