#pragma once

#include <functional>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/TcpConnection.h>
#include <mutex>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <unordered_map>
#include "friendmodel.hpp"
#include "groupmodel.hpp"
#include "offlinemessage.hpp"
#include "usermodel.hpp"

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

    // 添加好友
    void add_friend(const muduo::net::TcpConnectionPtr& , nlohmann::json&, muduo::Timestamp);

    // 一对一聊天 
    void one_chat(const muduo::net::TcpConnectionPtr&, nlohmann::json&, muduo::Timestamp);

    // 创建群聊
    void create_group(const muduo::net::TcpConnectionPtr&, nlohmann::json&, muduo::Timestamp);

    // 用户加入群聊
    void add_group(const muduo::net::TcpConnectionPtr&, nlohmann::json&, muduo::Timestamp);

    // 群聊
    void group_chat(const muduo::net::TcpConnectionPtr&, nlohmann::json&, muduo::Timestamp);

    // 客户端退出状态转换
    void client_close_exception(const muduo::net::TcpConnectionPtr& );
    
    // 重置所有用户状态
    void reset();

private:
    QChatService();

private:
    std::mutex _mtx;                                                                // 用于查询映射表的同步
    std::unordered_map<int, MsgHandler> _msg_handler_map;                           // 操作函数获取
    std::unordered_map<int, muduo::net::TcpConnectionPtr> _user_conn_map;           // 用户id：连接映射
    std::unordered_map<muduo::net::TcpConnectionPtr, int> _conn_user_map;           // 连接：用户id映射（用户客户端异常退出时处理）
    UserModel _usermodel;                                                           // 用户表操作
    OfflineMessage _offlinemessagemodel;                                            // 离线消息表操作
    GroupModel _groupmodel;                                                         // 群组表操作
    FriendModel _friendmodel;                                                       // 好友表操作
};