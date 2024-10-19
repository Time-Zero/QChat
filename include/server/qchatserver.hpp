#pragma once
#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

class QChatServer
{
public:
    QChatServer(muduo::net::EventLoop* loop, const muduo::net::InetAddress& listenAddr, const std::string& nameArg);

    void Start();

private:
    void OnConnection(const muduo::net::TcpConnectionPtr& conn);
    void OnMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buffer,muduo::Timestamp tt);

private:
    muduo::net::EventLoop* _loop;
    muduo::net::TcpServer _server;
};