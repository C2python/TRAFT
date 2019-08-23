#include "log.hpp"
#include "config.hpp"
#include "dout.hpp"

using namespace TRAFT;

int main(int argc, char* argv[]){
    


    cct->conf->_log->start();

    auto e = cct->conf->_log->create_entry(10,pthread_self(),"Test Log.");

    cct->conf->_log->submit_entry(e);

    dout(2)<<"Test Dout."<<dendl;

    dout(6)<<"Upper Level Log Level."<<dendl;

    lderr<<"ERROR Occur."<<dendl;

    cct->conf->_log->stop();

    cct->conf->_log->join();
    


    return 0;
}