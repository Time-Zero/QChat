#pragma once

enum EnMsgType
{
    REG_MSG = 1,                // 注册消息
    REG_MSG_ACK,                // 注册相应
    LOGIN_MSG,                  // 登录消息
    LOGIN_MSG_ACK,              // 登录相应
    ONE_CHAT_MSG,               // 一对一聊天
    ADD_FRIEND_MSG,              // 添加好友
};