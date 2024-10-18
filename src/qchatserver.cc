#include "qchatserver.hpp"
#include <functional>
#include <muduo/base/Logging.h>
#include <thread>

QChatServer::QChatServer(muduo::net::EventLoop* loop, const muduo::net::InetAddress& listenAddr, const std::string& nameArg)
    :_server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册连接回调
    _server.setConnectionCallback(std::bind(&QChatServer::OnConnection,this, std::placeholders::_1));
    
    // 注册消息回调
    _server.setMessageCallback(std::bind(&QChatServer::OnMessage,this, std::placeholders::_1, std::placeholders::_2,std::placeholders::_3));

    // 设置工作线程
    _server.setThreadNum(std::thread::hardware_concurrency());
}

void QChatServer::OnConnection(const muduo::net::TcpConnectionPtr& conn)
{
    // LOG_DEBUG << "OnConnection called";
    // 连接断开处理
    if(!conn->connected())
    {
        conn->shutdown();
    }
}


void QChatServer::OnMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buffer,muduo::Timestamp tt)
{
    // LOG_DEBUG << "OnMessage called";

}

void QChatServer::Start()
{
    _server.start();
}