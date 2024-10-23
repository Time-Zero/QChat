#pragma once

#include "user.hpp"
#include <string>

class GroupUser : public User
{
public:
    void SetRole(std::string role)
    {
        this->_role = role;
    }

private:
    std::string _role;
};