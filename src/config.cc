/*
* Author: zhangwen
* Feature: Global Conf Var
*/

#include "config.hpp"
#include <memory>

namespace TRAFT{

Context* cct= new Context();

int Context::set_log(const std::string& name){
    int r = conf->_log->set_log_file(name);
    return r;
}

void Context::set_loglevel(const int& level){
    conf->log_level = level;
}

}