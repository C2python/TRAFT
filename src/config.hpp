/*
* Author: zhangwen
* Feature: Global Conf Var
*/

#include <string>
#include "log.hpp"

#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

namespace TRAFT{

class Context;

extern Context* cct;

struct Context{
    struct M_Config_t{
        int log_level=5;
        int fd;
        std::shared_ptr<Log> _log;
        M_Config_t(){
            _log = std::make_shared<Log>("./traft.log");
        }
    };
    std::shared_ptr<M_Config_t> conf;
    Context(){
        conf = std::make_shared<M_Config_t>();

    }
    int set_log(const std::string&);
    void set_loglevel(const int&);
};

}

#endif