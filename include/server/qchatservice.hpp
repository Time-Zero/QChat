#pragma once

#include <functional>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/TcpConnection.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <unordered_map>

using MsgHandler = std::function<void(const muduo::net::TcpConnectionPtr& ,nlohmann::json& , muduo::Timestamp)>;
class QChatService
{
public:
    QChatService(const QChatService&) = delete ;
    static QChatService& GetInstance();

    // 获取msgid对应的处理方法
    MsgHandler GetHandler(int msgid);

    // 注册
    void reg(const muduo::net::TcpConnectionPtr& , nlohmann::json&, muduo::Timestamp);

    // 登录
    void login(const muduo::net::TcpConnectionPtr& , nlohmann::json&, muduo::Timestamp);
    
private:
    QChatService();

private:
    std::unordered_map<int, MsgHandler> _msg_handler_map;
};