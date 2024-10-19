#pragma once
#include <mysql/mysql.h>
#include <memory>
#include <string>

struct MysqlConnectDeleter
{
    void operator()(MYSQL* conn)
    {
        if(conn != nullptr)
        {
            mysql_close(conn);
        }
    }
};

class MySQL
{
public:
    MySQL();
    ~MySQL();

    // 连接数据库
    bool Connect();

    // 更新操作
    bool Update(const std::string& sql);
    
    // 查询操作
    MYSQL_RES* Query(const std::string& sql);

    // 获取数据库连接
    std::shared_ptr<MYSQL> GetConnection();
private:
    std::shared_ptr<MYSQL> _conn;
};