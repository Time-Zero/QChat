#include "qchatservice.hpp"
#include <functional>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <mutex>
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
    std::string name = js["name"];
    std::string password = js["password"];

    User user;
    user.SetName(name);
    user.SetPassword(password);

    bool ret = _usermodel.Insert(user);
    nlohmann::json response;
    response["msgid"] = REG_MSG_ACK;
    if(ret)
    {
        response["errno"] = 0;
        response["id"] = user.GetId();
    }
    else
    {
        response["errno"] = 1;
        response["errmsg"] = "register failed!";
    }

    conn->send(response.dump());
}

void QChatService::login(const muduo::net::TcpConnectionPtr& conn, nlohmann::json& js, muduo::Timestamp tt)
{
    // std::cout << "login is called" << std::endl;
    int id = js["id"].get<int>();
    std::string pwd = js["password"];

    // 查找用户
    User user = _usermodel.Query(id);
    
    nlohmann::json response;
    response["msgid"] = LOGIN_MSG_ACK;
    if(user.GetId() == id && user.GetPassword() == pwd)
    {
        // 如果用户名和密码都正确
        if(user.GetState() == "online")
        {
            // 用户在线
            response["errno"] = 2;
            response["errnomsg"] = "this account is online!";
        }
        else
        {
            {
                std::lock_guard<std::mutex> lck(_mtx);
                _user_conn_map.insert({id, conn});
            }

            // 用户状态设置为在线
            user.SetState("online");
            _usermodel.UpdateState(user);

            response["errno"] = 0;
            response["id"] = user.GetId();
            response["name"] = user.GetName();
        }
    }
    else
    {
        // 用户名或密码错误
        response["errno"] = 1;
        response["errmsg"] = "username or password error!";
    }

    conn->send(response.dump());
}

void QChatService::reset()
{
    _usermodel.ResetState();
}