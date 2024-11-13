#pragma once

#include "group.hpp"
#include <vector>

class GroupModel
{
public:
    // 创建群组
    bool CreateGroup(Group& group);
    // 加入群组
    void AddGroup(int userid, int groupid, std::string role);
    // 查找指定id的群聊
    Group SearchGroup(int groupid);
    // 查询用户所在群组的信息
    std::vector<Group> QueryGroup(int userid);

    // 根据指定groupid查询群组用户id列表，除了userid自己，用于用户群聊业务给其他用户发送消息
    std::vector<int> QueryGroupUsers(int userid, int gourpid);
};