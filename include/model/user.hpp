#pragma once
#include <cstdint>
#include <string>

// User表的ORM类
class User
{
public:
    User(uint32_t id = 0, std::string name = "", std::string password = "", std::string state = "")
        : _id(id), _name(name), _password(password), _state(state)
    {

    }

    void SetId(uint32_t id){_id = id;}
    void SetName(std::string name) {_name = name;}
    void SetPassword(std::string password) {_password = password;}
    void SetState(std::string state) {_state = state;}
    uint32_t GetId(){return _id;}
    std::string GetName() {return _name;}
    std::string GetPassword() {return _password;}
    std::string GetState() {return _state;}

private:
    uint32_t _id;
    std::string _name;
    std::string _password;
    std::string _state;
};