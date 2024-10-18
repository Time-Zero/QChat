#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include "qchatconfig.hpp"
using namespace std;

void ShowArgHelp()
{
    std::cout << "format: command -i <configfile>" << std::endl;
}

int main(int argc, char** argv)
{  
    if(argc < 2)
    {
        ShowArgHelp();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    std::string config_file;
    while((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c) {
        case 'i':
            config_file = optarg;
            break;
        default:
            ShowArgHelp();
            exit(EXIT_FAILURE);
        }
    }

    QChatConfig::GetInstance().LoadConfigFile(config_file);
    // std::cout << QChatConfig::GetInstance().Load("serverip") << std::endl;
    // std::cout << QChatConfig::GetInstance().Load("serverport") << std::endl;
    // std::cout << QChatConfig::GetInstance().Load("redisip") << std::endl;
    // std::cout << QChatConfig::GetInstance().Load("redisport") << std::endl;

    return 0;
}