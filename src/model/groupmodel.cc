#include "groupmodel.hpp"
#include <cstdio>
#include <cstdlib>
#include <mysql/mysql.h>
#include <strings.h>
#include <vector>
#include "db.hpp"
#include "group.hpp"
#include "groupuser.hpp"

bool GroupModel::CreateGroup(Group& group)
{
    char sql[1024];
    bzero(sql, sizeof(sql));
    sprintf(sql, "insert into AllGroup(groupname, groupdesc) values('%s','%s')"
                                                ,group.GetName().c_str(), group.GetDesc().c_str());

    MySQL mysql;
    if(mysql.Connect())
    {
        if(mysql.Update(sql))
        {
            group.SetId(mysql_insert_id(mysql.GetConnection().get()));
            return true;
        }
    }

    return false;
}

void GroupModel::AddGroup(int userid, int groupid, std::string role)
{
    char sql[1024];
    bzero(sql, sizeof(sql));
    sprintf(sql, "insert into GroupUser(groupid, userid, grouprole) values(%d,%d,'%s')",groupid, userid, role.c_str());

    MySQL mysql;
    if(mysql.Connect())
    {
        mysql.Update(sql);
    }
}

std::vector<Group> GroupModel::QueryGroup(int userid)
{
    char sql[1024];
    bzero(sql, sizeof(sql));
    sprintf(sql, "select a.id, a.groupname, a.groupdesc from AllGroup a inner join GroupUser b on a.id = b.groupid where b.userid = %d", userid);

    std::vector<Group> group_vec;
    MySQL mysql;
    if(mysql.Connect())
    {   
        // 查询该用户加入了哪些群
        MYSQL_RES* res = mysql.Query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            while(row != nullptr)
            {
                Group group;
                group.SetId(atoi(row[0]));
                group.SetName(row[1]);
                group.SetDesc(row[2]);

                group_vec.push_back(group);
                row = mysql_fetch_row(res);
            }

            mysql_free_result(res);
        }
    }

    // 查询这些群中都有哪些用户
    for(auto group : group_vec)
    {
        bzero(sql, sizeof(sql));
        sprintf(sql, "select a.id, a.name, a.state, b.grouprole from User a \
                                inner join GroupUser b on b.userid = a.id where b.groupid = %d", group.GetId());    
        MYSQL_RES *res = mysql.Query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            while(row != nullptr)
            {
                GroupUser user;
                user.SetId(atoi(row[0]));
                user.SetName(row[1]);
                user.SetState(row[2]);
                user.SetRole(row[3]);

                group.GetUsers().emplace_back(user);
                row = mysql_fetch_row(res);
            }
            mysql_free_result(res);
        }
    }

    return group_vec;
}

std::vector<int> GroupModel::QueryGroupUsers(int userid, int gourpid)
{
    char sql[1024];
    bzero(sql, sizeof(sql));
    sprintf(sql, "select userid from GroupUser where groupid = %d and userid != %d", gourpid, userid);

    std::vector<int> id_vec;
    MySQL mysql;
    if(mysql.Connect())
    {
        MYSQL_RES* res = mysql.Query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            while(row != nullptr)
            {
                id_vec.push_back(atoi(row[0]));
                row = mysql_fetch_row(res);
            }
            mysql_free_result(res);
        }
    }

    return id_vec;
}

Group GroupModel::SearchGroup(int groupid)
{
    char sql[1024];
    bzero(sql, sizeof(sql));
    sprintf(sql, "select groupname, groupdesc from AllGroup where id = %d", groupid);

    Group group;
    MySQL mysql;
    if(mysql.Connect())
    {
        MYSQL_RES* res = mysql.Query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row != nullptr)
            {
                group.SetName(row[0]);
                group.SetDesc(row[1]);
            }
            mysql_free_result(res);
        }
    }

    return group;
}