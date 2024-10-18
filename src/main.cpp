#include <cstdlib>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include "qchatconfig.hpp"
#include "qchatserver.hpp"
using namespace std;

void ShowArgHelp()
{
    std::cout << "format: command -i <configfile>" << std::endl;
}

int main(int argc, char** argv)
{  
    if(argc < 2)
    {
        ShowArgHelp();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    std::string config_file;
    while((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c) {
        case 'i':
            config_file = optarg;
            break;
        default:
            ShowArgHelp();
            exit(EXIT_FAILURE);
        }
    }

    // 读取配置文件
    QChatConfig::GetInstance().LoadConfigFile(config_file);

    // 启动muduo server
    muduo::net::EventLoop loop;
    muduo::net::InetAddress addr(QChatConfig::GetInstance().Load("serverip"), stoi(QChatConfig::GetInstance().Load("serverport")));

    QChatServer chat_sever(&loop, addr, "ChatServer");
    chat_sever.Start();
    loop.loop();

    return 0;
}