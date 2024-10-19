#pragma once

#include "user.hpp"

class UserModel
{
public:
    // 插入
    bool Insert(User& user);

    // 查询
    User Query(int id);

    // 更新状态
    bool UpdateState(User& user);

    // 重置所有用户状态
    void ResetState();
};