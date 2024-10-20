#include "qchatservice.hpp"
#include <functional>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include "public.hpp"
#include "user.hpp"
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
    _msg_handler_map.insert({ONE_CHAT_MSG, std::bind(&QChatService::one_chat,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    
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
                _conn_user_map.insert({conn, id});
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

void QChatService::one_chat(const muduo::net::TcpConnectionPtr& conn, nlohmann::json& js, muduo::Timestamp)
{
    int to_id = js["to"].get<int>();

    {
        std::lock_guard<std::mutex> lck(_mtx);
        auto it = _user_conn_map.find(to_id);
        if(it != _user_conn_map.end())
        {
            it->second->send(js.dump());
            return;
        }
    }

}

void QChatService::client_close_exception(const muduo::net::TcpConnectionPtr& conn)
{
    std::unique_lock<std::mutex> lck(_mtx);

    // 查找有没有对应连接
    auto it = _conn_user_map.find(conn);
    if(it != _conn_user_map.end())
    {
        int id = it->second;
        
        // 删除id：连接映射表和连接：id映射表中的关系
        auto user_it = _user_conn_map.find(id);
        _user_conn_map.erase(user_it);
        _conn_user_map.erase(it);
        lck.unlock();

        User user;
        user.SetId(id);
        user.SetState("offline");
        _usermodel.UpdateState(user);
    }
}
