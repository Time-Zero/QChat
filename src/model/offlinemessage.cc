#include "offlinemessage.hpp"
#include <cstdio>
#include <mysql/mysql.h>
#include <string>
#include <strings.h>
#include <vector>
#include "db.hpp"

void OfflineMessage::Insert(int userid, std::string msg)
{
    char sql[1024];
    bzero(sql, sizeof(sql));
    sprintf(sql, "insert into OfflineMessage(id, message) values(%d,'%s')",userid, msg.c_str());

    MySQL mysql;
    if(mysql.Connect())
    {
        mysql.Update(sql);
    }
}

void OfflineMessage::Remove(int userid)
{
    char sql[1024];
    bzero(sql, sizeof(sql));
    sprintf(sql, "delete from OfflineMessage where id = %d", userid);

    MySQL mysql;
    if(mysql.Connect())
    {
        mysql.Update(sql);
    }
}

std::vector<std::string> OfflineMessage::Query(int userid)
{
    char sql[1024];
    bzero(sql, sizeof(sql));
    sprintf(sql, "select message from OfflineMessage where id = %d", userid);

    std::vector<std::string> vec;
    MySQL mysql;
    if(mysql.Connect())
    {
        MYSQL_RES* res = mysql.Query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            while(row != nullptr)
            {
                vec.push_back(row[0]);
                row = mysql_fetch_row(res);
            }

            mysql_free_result(res);
        }
    }

    return vec;
}