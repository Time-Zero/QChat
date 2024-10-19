#include "qchatconfig.hpp"
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <iostream>
#include <muduo/base/Logging.h>
#include <string>
#include <strings.h>

QChatConfig::QChatConfig()
{

}

QChatConfig& QChatConfig::GetInstance()
{
    static QChatConfig instance;
    return instance;
}

QChatConfig::~QChatConfig()
{

}

void QChatConfig::LoadConfigFile(const std::string& config_file)
{
    FILE* pf = fopen(config_file.c_str(), "r");
    if(pf == nullptr)
    {
        std::cerr << "open config file failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    while(!feof(pf))
    {
        char buf[1024];
        bzero(buf, sizeof(buf));
        fgets(buf, sizeof(buf), pf);

        std::string str_buf(buf);
        // 去除多余的空格
        Trim(str_buf);

        // 如果是空行或者是注释行
        if(str_buf.empty() || str_buf[0] == '#')
        {
            continue;
        }

        // 如果行中没有‘=’
        int idx = str_buf.find('=');
        if(idx == -1) 
            continue;

        std::string key,value;
        key = str_buf.substr(0,idx);
        // 寻找‘=’之后的换行符
        int end_idx = str_buf.find('\n',idx);
        value = str_buf.substr(idx + 1, end_idx - idx - 1);
        Trim(key);
        Trim(value);
        _config_map.insert({key,value});
    }
}

void QChatConfig::Trim(std::string& str)
{
    int idx = str.find_first_not_of(" ");
    if(idx != -1)
    {
        str = str.substr(idx, str.size() - idx);
    }

    idx = str.find_last_not_of(" ");
    if(idx != -1)
    {
        str = str.substr(0, idx + 1);
    }

}

std::string QChatConfig::Load(const std::string &key)
{
    auto it = _config_map.find(key);
    if(it != _config_map.end())
    {
        return it->second;
    }

    return std::string("");    
}