#include "friendmodel.hpp"
#include <cstdio>
#include <cstdlib>
#include <mysql/mysql.h>
#include <strings.h>
#include <vector>
#include "db.hpp"
#include "user.hpp"

void FriendModel::Insert(int userid, int friendid)
{
    char sql[1024];
    bzero(sql,sizeof(sql));
    sprintf(sql, "insert into Friend(userid, friendid) values(%d,%d)",userid,friendid);

    MySQL mysql;
    if(mysql.Connect())
    {
        mysql.Query(sql);
    }
}

void FriendModel::Delete(int userid, int friendid)
{
    char sql[1024];
    bzero(sql, sizeof(sql));
    sprintf(sql, "delete from Friend where userid = %d and friendid = %d", userid, friendid);

    MySQL mysql;
    if(mysql.Connect())
    {
        mysql.Query(sql);
    }
}


std::vector<User> FriendModel::Query(int userid)
{
    char sql[1024];
    bzero(sql, sizeof(sql));
    sprintf(sql, 
            "select a.id, a.name, a.state from User a inner join Friend b on b.friendid = a.id where b.userid = %d,", userid);

    std::vector<User> vec;
    MySQL mysql;
    if(mysql.Connect())
    {
        MYSQL_RES* res = mysql.Query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            while(row != nullptr)
            {
                User user;
                user.SetId(atoi(row[0]));
                user.SetName(row[1]);
                user.SetState(row[2]);

                vec.push_back(user);
                row = mysql_fetch_row(res);
            }

            mysql_free_result(res);
        }
    }

    return vec;
}