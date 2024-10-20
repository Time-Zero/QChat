#include "qchatserver.hpp"
#include "qchatservice.hpp"
#include <functional>
#include <muduo/base/Logging.h>
#include <nlohmann/json_fwd.hpp>
#include <thread>
#include <nlohmann/json.hpp>

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
        QChatService::GetInstance().client_close_exception(conn);
        conn->shutdown();
    }
}

// 处理消息事件的回调函数
void QChatServer::OnMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buffer,muduo::Timestamp tt)
{
    
    // LOG_DEBUG << "OnMessage called";
    std::string buf = buffer->retrieveAllAsString();
    nlohmann::json js = nlohmann::json::parse(buf);

    // 检查是否有'msgid'字段
    if(js.contains("msgid"))
    {
        auto handler = QChatService::GetInstance().GetHandler(js["msgid"].get<int>());
        handler(conn, js, tt);
    }
    else
    {
        LOG_ERROR << "the json content you send, do not have 'msgid' key!";
    }

}

void QChatServer::Start()
{
    _server.start();
}