#include "Log.hpp"
#include "Config.hpp"
#include <thread>


int main(int argc, char* argv[]){
    


    TRAFT::cct->conf->_log->start();

    auto e = TRAFT::cct->conf->_log->create_entry(10,std::this_thread::get_id(),"Test Log");

    TRAFT::cct->conf->_log->submit_entry(e);

    TRAFT::cct->conf->_log->stop();

    TRAFT::cct->conf->_log->join();


    return 0;
}