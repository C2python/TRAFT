#include "log.hpp"
#include "config.hpp"
#include "dout.hpp"
#include "Timer.hpp"

using namespace TRAFT;
//extern Context* cct;

int global_init(){
    cct = new Context();
    cct->conf->_log->start();
    return 1;
}

int main(int argc, char* argv[]){
    
    int r = global_init();
    /*
    auto e = cct->conf->_log->create_entry(10,pthread_self(),"Test Log.");

    cct->conf->_log->submit_entry(e);
    */
    dout(2)<<"Test Dout AFter."<<dendl;

    dout(2)<<"Test Dout."<<dendl;

    dout(6)<<"Upper Level Log Level."<<dendl;

    lderr<<"ERROR Occur."<<dendl;

    std::shared_ptr<SafeTimer> timer = std::make_shared<SafeTimer>();
    timer->init();

    timer->add_event_after(5,new TestContext("Test1"));

    timer->add_event_after(6,new TestContext("Test2"));

    timer->add_event_after(10,new TestContext("Test3"));

    sleep(14);

    timer->shutdown();

    cct->conf->_log->stop();
    
    cct->conf->_log->join();


    return 0;
}