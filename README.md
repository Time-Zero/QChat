# QChat——基于C++的集群聊天系统
## 简介
基于C++的集群聊天系统，使用MySQL作为后端数据库，redis进行服务器间通信，nginx进行服务器间负载均衡
## 服务器
服务器使用muduo库处理网络层任务，使用MySQL作为后端数据库，使用libmysqlclient作为通信api，使用hiredis连接redis。使用Cmake编译项目，编译前请确认是否安装libmysqlclient和hiredis
### 启动命令
```shell
./bin/QChat -i test.conf
```
## 客户端
客户端使用Qt构建，代码放在QChat.zip中。使用Qt6.6.3，使用mingw作为编译器