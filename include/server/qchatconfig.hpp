#pragma once

#include <unordered_map>
#include <string>

class QChatConfig
{
public:
    QChatConfig(const QChatConfig&) = delete;
    // 获取单例
    static QChatConfig& GetInstance();
    // 加载配置文件
    void LoadConfigFile(const std::string& config_file);
    // 去空格
    void Trim(std::string& str);
    // 获取配置项
    std::string Load(const std::string& key);

private:
    QChatConfig();
    ~QChatConfig();

private:
    std::unordered_map<std::string, std::string> _config_map;
};