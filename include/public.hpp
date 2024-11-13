#pragma once

enum EnMsgType
{
    REG_MSG = 1,                // 注册消息
    REG_MSG_ACK,                // 注册相应
    LOGIN_MSG,                  // 登录消息
    LOGIN_MSG_ACK,              // 登录相应
    ONE_CHAT_MSG,               // 一对一聊天
    ADD_FRIEND_MSG,              // 添加好友
    CREAT_GROUP_MSG,           // 创建群组
    ADD_GROUP_MSG,               // 加入群组
    GROUP_CHAT_MSG,             // 群聊
    LOGIN_OUT_MSG,              // 注销
    DELETE_FRIEND_MSG,          // 删除好友
    USER_INFO_EDIT_MSG,         // 用户信息修改
    GROUP_SEARCH_MSG,           // 通过id查找群
    GROUP_SEARCH_ACK,           // 发送id查找群的结果
};