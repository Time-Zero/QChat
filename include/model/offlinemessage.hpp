#pragma once

#include <string>
#include <vector>

class OfflineMessage
{
public:
    // 添加离线消息
    void Insert(int userid, std::string msg);

    // 移除一个用户的离线消息
    void Remove(int userid);

    // 获取一个用户的全部离线消息
    std::vector<std::string> Query(int userid);
};