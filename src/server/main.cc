#include "../../include/server/ChatServer.h"
#include "../../include/server/ChatService.h"

#include <signal.h>

using namespace std;

void resetHandler(int)
{
    ChatService::getInstance()->reset();
    exit(0);
}

int main(int argc, const char * argv[])
{
    string ip = argv[1];
    string portStr = argv[2];
    unsigned short port = atoi(portStr.c_str());

    signal(SIGINT, resetHandler);

    EventLoop loop;
    // InetAddress addr("127.0.0.1", 6060);
    InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "ChatServer");
    server.start();
    loop.loop();
    return 0;
}
