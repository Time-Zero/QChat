#include "qchatservice.hpp"
#include <functional>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <nlohmann/json_fwd.hpp>
#include "public.hpp"
// #include <iostream>

QChatService& QChatService::GetInstance()
{
    static QChatService instance;
    return instance;
}

QChatService::QChatService()
{
    _msg_handler_map.insert({REG_MSG, std::bind(&QChatService::reg,this,std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    _msg_handler_map.insert({LOGIN_MSG, std::bind(&QChatService::login,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
}

MsgHandler QChatService::GetHandler(int msgid)
{
    auto it = _msg_handler_map.find(msgid);
    if(it == _msg_handler_map.end())
    {
        return [=](const muduo::net::TcpConnectionPtr& conn, nlohmann::json& js, muduo::Timestamp){
            LOG_ERROR << "msgid:" << msgid << " can not find handler!";
        };
    }
    else
    {
        return it->second;
    }
}

void QChatService::reg(const muduo::net::TcpConnectionPtr& conn, nlohmann::json& js, muduo::Timestamp tt)
{
    // std::cout << "reg is called" << std::endl;
}

void QChatService::login(const muduo::net::TcpConnectionPtr& conn, nlohmann::json& js, muduo::Timestamp tt)
{
    // std::cout << "login is called" << std::endl;
}