#pragma once
#include <functional>
#include <hiredis/hiredis.h>
#include <string>

class Redis {
public:
    Redis();
    ~Redis();

    // 连接到redis
    bool connect();

    // 向指定通道publish消息
    bool publish(int channel, std::string message);

    // 订阅消息
    bool subscribe(int channel);

    // 取消订阅
    bool unsubscribe(int channel);

    // 在独立线程中接收订阅通道中的消息
    void observer_channel_message();

    // 初始化向业务层上报通信消息的回调对象
    void init_notify_handler(std::function<void(int, std::string)> fn);

private:
    // publish消息上下文
    redisContext* _publish_context;
    // subscribe消息上下文
    redisContext* _subscribe_context;
    // 回调操作，收到订阅的消息，向service层上报
    std::function<void(int, std::string)> _notify_message_handler;
};
