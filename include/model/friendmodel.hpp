#pragma once

#include "user.hpp"
#include <vector>
class FriendModel
{
public:
    // 添加好友
    void Insert(int userid, int friendid);

    // 删除好友
    void Delete(int userid, int friendid);

    // 返回用户好友关系(friendid + friendname)
    std::vector<User> Query(int userid);
};