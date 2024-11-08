#include "db.hpp"
#include <cstdint>
#include <muduo/base/Logging.h>
#include <mysql/mysql.h>
#include <qchatconfig.hpp>
#include <string>

MySQL::MySQL() : _conn(mysql_init(nullptr), MysqlConnectDeleter())
{

}

MySQL::~MySQL()
{
    _conn.reset();
}

bool MySQL::Connect()
{
    std::string host = QChatConfig::GetInstance().Load("mysqlip");
    uint32_t port = std::stoi(QChatConfig::GetInstance().Load("mysqlport"));
    std::string user_name = QChatConfig::GetInstance().Load("mysqlusername");
    std::string user_pwd = QChatConfig::GetInstance().Load("mysqlpassword");
    std::string dbname = QChatConfig::GetInstance().Load("mysqldbname");

    MYSQL* p = mysql_real_connect(_conn.get(), host.c_str(), user_name.c_str()
                                        , user_pwd.c_str(), dbname.c_str(), port,nullptr, 0);

    if(p == nullptr)
    {
        LOG_ERROR << "connect mysql failed";
    }
    else
    {
        // mysql_query(_conn.get(), "set names gbk");      // 让代码可以支持中文
        mysql_query(_conn.get(), "use chat");           // 切换到chat数据库
        LOG_INFO << "connect mysql success";
    }

    return p;
}

bool MySQL::Update(const std::string& sql)
{
    if(mysql_query(_conn.get(), sql.c_str()))
    {
        LOG_ERROR << __FILE__ << ":" << __LINE__ << ":" << sql << " failed!";
        return false;
    }

    return true;
}

MYSQL_RES* MySQL::Query(const std::string& sql)
{
    if(Update(sql))
    {
        return mysql_use_result(_conn.get());
    }

    return nullptr;
}

std::shared_ptr<MYSQL> MySQL::GetConnection()
{
    return this->_conn;
}