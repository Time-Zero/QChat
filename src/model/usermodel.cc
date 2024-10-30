#include "usermodel.hpp"
#include <cstdio>
#include <cstdlib>
#include <strings.h>
#include "db.hpp"
#include "user.hpp"

bool UserModel::Insert(User& user)
{
    char sql[1024];
    bzero(sql, sizeof(sql));
    sprintf(sql, "insert into User(name, password, state) values('%s', '%s', '%s')", 
                            user.GetName().c_str(), user.GetPassword().c_str(), user.GetState().c_str());
    
    MySQL mysql;
    if(mysql.Connect())
    {
        if(mysql.Update(sql))
        {
            // 获取新注册的用户的id
            user.SetId(mysql_insert_id(mysql.GetConnection().get()));
            return true;
        }
    }

    return false;
}

User UserModel::Query(int id)
{
    char sql[1024];
    bzero(sql, sizeof(sql));
    sprintf(sql, "select id, name, password, state from User where id = %d", id);

    MySQL mysql;
    if(mysql.Connect())
    {
        MYSQL_RES* res = mysql.Query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row != nullptr)
            {
                User user(atoi(row[0]), row[1], row[2], row[3]);
                mysql_free_result(res);
                return user;
            }

            mysql_free_result(res);
        }
    }

    return User();
}

bool UserModel::UpdateState(User& user)
{
    char sql[1024];
    bzero(sql, sizeof(sql));
    sprintf(sql, "update User set state = '%s' where id = %d", user.GetState().c_str(), user.GetId());

    MySQL mysql;
    if(mysql.Connect())
    {
        if(mysql.Update(sql))
        {
            return true;
        }
    }

    return false;
}

void UserModel::ResetState()
{
    std::string sql = "update User set state = 'offline' where state = 'online'";

    MySQL mysql;
    if(mysql.Connect())
    {
        mysql.Update(sql);
    }
}  

void UserModel::EditUserInfo(User& user)
{
    char sql[1024];
    bzero(sql, sizeof(sql));
    sprintf(sql, "update User set name = '%s', password = '%s' where id = %d"
            , user.GetName().c_str(), user.GetPassword().c_str(), user.GetId());

    MySQL mysql;
    if(mysql.Connect())
    {
        mysql.Update(sql);
    }
}