#include <gtest/gtest.h>
#include <iostream>

#include "../src/log.hpp"
#include "../src/config.hpp"
#include "../src/dout.hpp"
using namespace TRAFT;

TEST(Log,fun_set){
    std::shared_ptr<Log> log = std::make_shared<Log>("/var/log/traft.log");
    log->start();
    auto e = log->create_entry(4,pthread_self(),"Test Log.");
    log->submit_entry(e);
    log->stop();
    log->join();
}

class LogTest: public ::testing::Test{
protected:
    static void SetUpTestCase() {
        cct = new Context();
        cct->conf->_log->start();
    }

    static void TearDownTestCase() {
        cct->conf->_log->stop();
        cct->conf->_log->join();
    }

};

TEST_F(LogTest,dout){
    dout(1)<<"Test Log Dout Fun."<<dendl;
    lderr<<"Error Log."<<dendl;
}

TEST_F(LogTest,set_log){
    cct->set_log("/var/log/ttraft.log");
    EXPECT_EQ(cct->conf->_log->get_log_file(),"/var/log/ttraft.log");
    lderr<<"Error Log."<<dendl;
}

TEST_F(LogTest,set_log_level){
    cct->set_loglevel(9);
    EXPECT_EQ(cct->conf->log_level,9);
}