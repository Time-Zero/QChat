#include "qchatservice.hpp"
#include <functional>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <vector>
#include "group.hpp"
#include "public.hpp"
#include "user.hpp"

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
    _msg_handler_map.insert({ADD_FRIEND_MSG, std::bind(&QChatService::add_friend,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    _msg_handler_map.insert({CREAT_GROUP_MSG, std::bind(&QChatService::create_group,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    _msg_handler_map.insert({ADD_GROUP_MSG, std::bind(&QChatService::add_group,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    _msg_handler_map.insert({GROUP_CHAT_MSG, std::bind(&QChatService::group_chat,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    _msg_handler_map.insert({LOGIN_OUT_MSG, std::bind(&QChatService::login_out,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
    _msg_handler_map.insert({DELETE_FRIEND_MSG, std::bind(&QChatService::delete_friend,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3)});
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
            response["errnomsg"] = "当前账户已被登录!";
        }
        else        // 正常登录处理逻辑
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

            // 查询登录用户是否有离线消息
            auto vec = _offlinemessagemodel.Query(user.GetId());
            if(!vec.empty())
            {
                response["offlinemsg"] = vec;
                _offlinemessagemodel.Remove(user.GetId());
            }    

            std::vector<User> user_vec = _friendmodel.Query(user.GetId());
            if(!user_vec.empty())
            {
                std::vector<std::string> temp_vec;
                for(auto it : user_vec)
                {
                    nlohmann::json user_js;
                    user_js["id"] = it.GetId();
                    user_js["name"] = it.GetName();
                    user_js["state"] = it.GetState();
                    temp_vec.emplace_back(user_js.dump());
                }
                response["friends"] = temp_vec; 
            }
        }
    }
    else
    {
        // 用户名或密码错误
        response["errno"] = 1;
        response["errmsg"] = "账号或密码错误!";
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

    User user = _usermodel.Query(to_id);
    if(user.GetState() == "online")
    {
        // 如果在线通过redis转发
        // TODO: 添加redis在线转发
    }
    else
    {
        _offlinemessagemodel.Insert(to_id, js.dump());
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

void QChatService::add_friend(const muduo::net::TcpConnectionPtr& conn, nlohmann::json& js, muduo::Timestamp)
{
    int userid = js["id"];
    int friendid = js["friendid"];

    _friendmodel.Insert(userid, friendid);
}

void QChatService::delete_friend(const muduo::net::TcpConnectionPtr& conn, nlohmann::json& js, muduo::Timestamp)
{
    int userid = js["id"];
    int friendid = js["friendid"];

    _friendmodel.Delete(userid, friendid);
}

void QChatService::create_group(const muduo::net::TcpConnectionPtr& conn, nlohmann::json& js, muduo::Timestamp)
{
    int userid = js["id"];
    std::string name = js["groupname"];
    std::string desc = js["groupdesc"];

    Group group(0, name, desc);
    if(_groupmodel.CreateGroup(group))      // 创建群组
    {
        _groupmodel.AddGroup(userid, group.GetId(), "creator");     // 向群组中添加用户，并且设置创建用户的身份为creator
    }
}

void QChatService::add_group(const muduo::net::TcpConnectionPtr& conn, nlohmann::json& js, muduo::Timestamp)
{
    int userid = js["id"];
    int groupid = js["groupid"];

    _groupmodel.AddGroup(userid, groupid, "normal");
}

void QChatService::group_chat(const muduo::net::TcpConnectionPtr& conn, nlohmann::json& js, muduo::Timestamp)
{
    int userid = js["id"];
    int groupid = js["groupid"];

    std::vector<int> userid_vec = _groupmodel.QueryGroupUsers(userid, groupid);
    for(auto id : userid_vec)
    {
        std::unique_lock<std::mutex> lck(_mtx);
        auto it = _user_conn_map.find(id);
        if(it != _user_conn_map.end())
        {
            it->second->send(js.dump());
            lck.unlock();
        }
        else
        {
            lck.unlock();
            User user = _usermodel.Query(id);
            if(user.GetState() == "online")
            {
                // 用户在线，但是不在本服务器登录
                // TODO:使用redis转发消息
            }
            else
            {
                // 如果用户离线，就保存用户离线消息
                _offlinemessagemodel.Insert(id, js.dump());
            }
        }
    }
}

void QChatService::login_out(const muduo::net::TcpConnectionPtr& conn, nlohmann::json& js, muduo::Timestamp)
{
    int userid = js["id"];

    {
        std::lock_guard<std::mutex> lck(_mtx);
        auto it = _user_conn_map.find(userid);
        if(it != _user_conn_map.end())
        {
            _conn_user_map.erase(it->second);
            _user_conn_map.erase(it);
        }
    }

    // TODO:注销redis

    User user(userid,"","","offline");
    _usermodel.UpdateState(user);
}