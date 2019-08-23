/*
* Author: zhangwen
* Feature: Create a static instance to get callbacks called when process exited via main() or exit()
*/

#ifndef ON_EXIT_H
#define ON_EXIT_H

#include <functional>
#include <mutex>
#include <vector>

namespace TRAFT{

struct OnExitManager{

    using callback_t = std::function<void(void *arg)>;

    OnExitManager(){}
    ~OnExitManager(){
        
        std::lock_guard<std::mutex> lock(call_mutex);
        for (auto& cb:funcs){
            cb.func(cb.arg);
        }
        std::vector<callback>().swap(funcs);
    }
    void add_callback(callback_t func,void *arg){
        callback cb={func,arg};
        std::lock_guard<std::mutex> lock(call_mutex);
        funcs.push_back(cb);
    }

private:

    struct callback{
        callback_t func;
        void *arg;
    };
    std::mutex call_mutex;
    std::vector<callback> funcs;

};

}

#endif

