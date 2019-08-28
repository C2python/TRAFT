#include <gtest/gtest.h>
#include <iostream>

#include "log.hpp"
#include "config.hpp"
#include "dout.hpp"
using namespace TRAFT;

TEST(Log,fun_set){
    std::shared_ptr<Log> log = std::make_shared<Log>("/var/log/traft.log");
    log->start();
    auto e = log->create_entry(4,pthread_self(),"Test Log.");
    log->submit_entry(e);
    log->stop();
    log->join();
}

TEST(Log,dout){
    cct->conf->_log->start();
    dout(1)<<"Test Log Dout Fun."<<dendl;
    lderr<<"Error Log."<<dendl;
    cct->conf->_log->stop();
    cct->conf->_log->join();
}

TEST(Context,set_log){
    cct->conf->_log->start();
    cct->set_log("/var/log/ttraft.log");
    EXPECT_EQ(cct->conf->_log->get_log_file(),"/var/log/ttraft.log");
    lderr<<"Error Log."<<dendl;
    cct->conf->_log->stop();
    cct->conf->_log->join();
}

TEST(Context,set_log_level){
    cct->set_loglevel(9);
    EXPECT_EQ(cct->conf->log_level,9);
}