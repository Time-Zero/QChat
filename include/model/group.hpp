#pragma once

#include "groupuser.hpp"
#include <string>
#include <vector>

class Group
{
public:
    Group(unsigned int id = 0, std::string name = "", std::string desc = "")
        : _id(id), _name(name), _desc(desc)
    {
    
    }

    void SetId(unsigned int id)
    {
        this->_id = id;
    }

    void SetName(std::string name)
    {
        this->_name = name;
    }

    void SetDesc(std::string desc)
    {
        this->_desc = desc;
    }

    unsigned int GetId()
    {
        return this->_id;
    }

    std::string GetName()
    {
        return this->_name;
    }

    std::string GetDesc()
    {
        return this->_desc;
    }

    std::vector<GroupUser>& GetUsers()
    {
        return this->_users;
    }

private:
    unsigned int _id;
    std::string _name;
    std::string _desc;
    std::vector<GroupUser> _users;
};