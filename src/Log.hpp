#ifndef TRAFT_LOG_H
#define TRAFT_LOG_H

#include<queue>
#include<mutex>
#include<thread>
#include<condition_variable>
#include<atomic>
#include<cassert>
#include<string>
#include<iostream>

#include "On_exit.hpp"
#include "Util.hpp"
#include "Entry.hpp"

namespace TRAFT{

class Log{
    
private:

    int m_fd; //Log file descriptor

    Log** m_exit;

    std::queue<std::shared_ptr<Entry>> m_new;
    std::queue<std::shared_ptr<Entry>> m_recent;
    int max_new;
    int max_recent;

    std::string m_log_file;
    uid_t m_uid;
    gid_t m_gid;

    std::atomic<bool> m_stop{true};

    std::thread logHandle;
    std::mutex lock_queue;
    std::mutex lock_flush;
    std::condition_variable cond_queue;
    std::condition_variable cond_flush;

    pthread_t m_queue_mutex_holder;
    pthread_t m_flush_mutex_holder;

    void _flush(std::queue<std::shared_ptr<Entry>>&,bool crash=false);

public:

    Log(const std::string&);
    ~Log();
    void set_flush_on_exit();
    void flush();
    void start();
    void stop();
    bool is_started();
    void open_log_file();
    void thread_handle(std::string name);
    void join();
    void detach();
    std::shared_ptr<Entry> create_entry(int level,std::thread::id,const std::string&);
    void submit_entry(std::shared_ptr<Entry>);
    int set_log_file(const std::string& log_file);

};

}

#endif